#!/usr/bin/env python3
"""Build and consume metadata for Kingdom Hearts I script commands."""

from __future__ import annotations

import argparse
import hashlib
import json
import mmap
import re
import struct
import sys
from collections import Counter, defaultdict
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable, Iterator

if __package__:
    from .script_bytecode import FormatError, Instruction, ScriptBlock, find_script_blocks
else:
    from script_bytecode import FormatError, Instruction, ScriptBlock, find_script_blocks

COMMAND_OPCODE = 24
DEFAULT_TABLE_SYMBOL = "D_003D4A20"
DEFAULT_FILENAMES = Path(__file__).with_name("iso") / "kingdom_filenames.txt"
ISO_BLOCK_SIZE = 0x800


@dataclass(frozen=True)
class StackSignature:
    inputs: int | None
    outputs: int | None
    net: int | None
    confidence: str
    evidence: str | None = None


@dataclass(frozen=True)
class CommandMetadata:
    command_id: int
    handler_address: int
    handler_symbol: str | None
    name: str | None
    name_confidence: str
    stack: StackSignature
    call_count: int

    @property
    def display_name(self) -> str:
        return self.name or f"command_{self.command_id}"


def load_command_database(path: Path) -> dict[int, CommandMetadata]:
    document = json.loads(path.read_text())
    if document.get("schema_version") != 1:
        raise FormatError(f"unsupported command database schema in {path}")

    commands: dict[int, CommandMetadata] = {}
    for item in document.get("commands", []):
        command_id = item["id"]
        if command_id in commands:
            raise FormatError(f"duplicate command {command_id} in {path}")
        stack_item = item.get("stack", {})
        stack = StackSignature(
            stack_item.get("inputs"),
            stack_item.get("outputs"),
            stack_item.get("net"),
            stack_item.get("confidence", "unknown"),
            stack_item.get("evidence"),
        )
        commands[command_id] = CommandMetadata(
            command_id,
            int(item["handler_address"], 0),
            item.get("handler_symbol"),
            item.get("name"),
            item.get("name_confidence", "unknown"),
            stack,
            item.get("call_count", 0),
        )
    return commands


def _read_elf32_virtual(data: bytes, address: int, size: int) -> bytes:
    if data[:6] != b"\x7fELF\x01\x01":
        raise FormatError("expected a little-endian ELF32 executable")
    program_offset = struct.unpack_from("<I", data, 0x1C)[0]
    program_size = struct.unpack_from("<H", data, 0x2A)[0]
    program_count = struct.unpack_from("<H", data, 0x2C)[0]
    for index in range(program_count):
        offset = program_offset + index * program_size
        segment_type, file_offset, virtual_address, _, file_size, _, _, _ = struct.unpack_from(
            "<8I", data, offset
        )
        if segment_type != 1:
            continue
        relative = address - virtual_address
        if 0 <= relative and relative + size <= file_size:
            return data[file_offset + relative : file_offset + relative + size]
    raise FormatError(f"virtual range 0x{address:08X}..0x{address + size:08X} is not file-backed")


def _parse_linker_map(path: Path) -> tuple[dict[str, int], dict[int, list[str]]]:
    by_name: dict[str, int] = {}
    by_address: dict[int, list[str]] = defaultdict(list)
    pattern = re.compile(r"^\s+0x([0-9A-Fa-f]+)\s+([A-Za-z_.$][\w.$]*)\s*$")
    for line in path.read_text(errors="replace").splitlines():
        match = pattern.match(line)
        if not match:
            continue
        address = int(match.group(1), 16)
        name = match.group(2)
        by_name.setdefault(name, address)
        by_address[address].append(name)
    return by_name, by_address


def _preferred_symbol(names: Iterable[str]) -> str | None:
    candidates = [name for name in names if not name.endswith(".NON_MATCHING")]
    if not candidates:
        return None
    return min(
        candidates,
        key=lambda name: (
            name.startswith("D_"),
            name.startswith("func_"),
            len(name),
            name,
        ),
    )


def _infer_direct_stack_inputs(code: bytes) -> int | None:
    """Infer inputs for a direct stack pop or a stack-free leaf handler."""
    context_registers = {4}
    stack_offsets: dict[int, int] = {}
    stored_offsets: set[int] = set()
    caller_saved = set(range(2, 16)) | {24, 25}
    immediate_writes = set(range(8, 16)) | {24, 25}
    load_opcodes = set(range(32, 40)) | {26, 27, 55}
    register_functions = set(range(8)) | {10, 11} | set(range(32, 48))
    loaded_stack_top = False
    calls_handler = False

    for (word,) in struct.iter_unpack("<I", code[: len(code) & ~3]):
        opcode = word >> 26
        source = (word >> 21) & 31
        target = (word >> 16) & 31
        destination = (word >> 11) & 31
        function = word & 63
        immediate = word & 0xFFFF
        signed_immediate = immediate - 0x10000 if immediate & 0x8000 else immediate
        old_context = set(context_registers)
        old_offsets = dict(stack_offsets)

        if opcode == 0 and function in (0x21, 0x25, 0x2D):
            context_registers.discard(destination)
            stack_offsets.pop(destination, None)
            copied_register = source if target == 0 else target if source == 0 else None
            if copied_register is not None:
                if copied_register in old_context:
                    context_registers.add(destination)
                if copied_register in old_offsets:
                    stack_offsets[destination] = old_offsets[copied_register]
        elif opcode in (9, 25):
            context_registers.discard(target)
            stack_offsets.pop(target, None)
            if signed_immediate == 0 and source in old_context:
                context_registers.add(target)
            if source in old_offsets:
                stack_offsets[target] = old_offsets[source] + signed_immediate
        elif opcode == 35:
            context_registers.discard(target)
            stack_offsets.pop(target, None)
            if signed_immediate == 0x180 and source in old_context:
                loaded_stack_top = True
                stack_offsets[target] = 0
        elif opcode == 43:
            if (
                signed_immediate == 0x180
                and source in old_context
                and target in old_offsets
            ):
                stored_offsets.add(old_offsets[target])
        else:
            written_register = None
            if opcode == 0 and function in register_functions:
                written_register = destination
            elif opcode in immediate_writes or opcode in load_opcodes:
                written_register = target
            if written_register is not None:
                context_registers.discard(written_register)
                stack_offsets.pop(written_register, None)
            is_call = (
                opcode == 3
                or (opcode == 0 and function == 9)
                or (opcode == 1 and target in (16, 17))
            )
            if is_call:
                calls_handler = True
                context_registers.difference_update(caller_saved)
                for register in caller_saved:
                    stack_offsets.pop(register, None)

    if len(stored_offsets) == 1:
        stack_delta = next(iter(stored_offsets))
        if stack_delta < 0:
            return -stack_delta
    if not stored_offsets and not loaded_stack_top and not calls_handler:
        return 0
    return None


def _hash_filename(filename: str) -> int:
    value = 0
    for character in filename:
        value = ((value * 2) ^ ((ord(character) << 16) % 69665)) & 0xFFFFFFFF
    return value


def _iso_file_block(image: mmap.mmap, filename: str) -> int:
    target = filename.encode()
    position = 0x105 * ISO_BLOCK_SIZE
    for _ in range(0x500):
        if image[position] == 1:
            length = image[position + 1]
            if length == len(target) and image[position + 2 : position + 2 + length] == target:
                return struct.unpack_from(">I", image, position - 0x19)[0]
        position += 1
    raise FormatError(f"{filename} is absent from ISO filesystem")


def _decompress(data: bytes) -> bytes:
    source_index = len(data) - 1
    if source_index <= 0:
        return b""
    key = data[source_index]
    source_index -= 1
    size = data[source_index] | data[source_index - 1] << 8 | data[source_index - 2] << 16
    source_index -= 3
    destination = bytearray(size)
    destination_index = size - 1
    while destination_index >= 0 and source_index >= 0:
        value = data[source_index]
        source_index -= 1
        if value == key and source_index >= 0:
            copy_offset = data[source_index]
            source_index -= 1
            if copy_offset > 0 and source_index >= 0:
                copy_length = data[source_index]
                source_index -= 1
                for _ in range(copy_length + 3):
                    source = destination_index + copy_offset
                    destination[destination_index] = destination[source] if source < len(destination) else 0
                    destination_index -= 1
                    if destination_index < 0:
                        break
            else:
                destination[destination_index] = value
                destination_index -= 1
        else:
            destination[destination_index] = value
            destination_index -= 1
    if destination_index >= 0:
        raise FormatError("compressed file ended before its declared output size")
    return bytes(destination)


def _load_filename_map(path: Path) -> dict[int, str]:
    return {
        _hash_filename(name): name
        for line in path.read_text().splitlines()
        if (name := line.strip())
    }


def _iter_iso_ards(path: Path, filenames_path: Path) -> Iterator[tuple[str, bytes]]:
    filenames = _load_filename_map(filenames_path)
    with path.open("rb") as stream:
        image = mmap.mmap(stream.fileno(), 0, access=mmap.ACCESS_READ)
        try:
            index_block = _iso_file_block(image, "KINGDOM.IDX;1")
            base_block = _iso_file_block(image, "SYSTEM.CNF;1")
            position = index_block * ISO_BLOCK_SIZE
            while True:
                name_hash, compressed, block, length = struct.unpack_from("<4I", image, position)
                position += 16
                if name_hash == 0:
                    break
                name = filenames.get(name_hash)
                if name is None or not name.lower().endswith(".ard"):
                    continue
                start = (base_block + block) * ISO_BLOCK_SIZE
                contents = bytes(image[start : start + length])
                yield name, _decompress(contents) if compressed else contents
        finally:
            image.close()


def iter_input_ards(paths: Iterable[Path], filenames_path: Path = DEFAULT_FILENAMES) -> Iterator[tuple[str, bytes]]:
    for path in paths:
        if path.suffix.lower() == ".iso":
            yield from _iter_iso_ards(path, filenames_path)
        elif path.suffix.lower() == ".ard":
            yield path.name, path.read_bytes()
        else:
            raise FormatError(f"unsupported corpus input {path}; expected .ard or .iso")


def _profile_corpus(
    paths: Iterable[Path], filenames_path: Path
) -> tuple[dict[str, int], Counter[int], dict[int, list[dict[str, int | str]]]]:
    frequency: Counter[int] = Counter()
    examples: dict[int, list[dict[str, int | str]]] = defaultdict(list)
    totals = Counter()
    for archive_name, data in iter_input_ards(paths, filenames_path):
        blocks = tuple(find_script_blocks(data))
        if not blocks:
            continue
        totals["archives"] += 1
        totals["blocks"] += len(blocks)
        for block in blocks:
            for script in block.scripts:
                totals["scripts"] += 1
                totals["entries"] += len(script.entries)
                instruction_index = 0
                for entry_index, entry in enumerate(script.entries):
                    for instruction in entry:
                        totals["instructions"] += 1
                        if instruction.opcode == COMMAND_OPCODE:
                            command_id = instruction.operand
                            frequency[command_id] += 1
                            if len(examples[command_id]) < 3:
                                examples[command_id].append(
                                    {
                                        "archive": archive_name,
                                        "resource": block.resource_index,
                                        "block": block.block_index,
                                        "script": script.script_index,
                                        "entry": entry_index,
                                        "instruction": instruction_index,
                                    }
                                )
                        instruction_index += 1
    totals["command_calls"] = sum(frequency.values())
    totals["commands_used"] = len(frequency)
    return dict(totals), frequency, examples


def _load_semantics(path: Path | None) -> dict[int, dict[str, object]]:
    if path is None:
        return {}
    document = json.loads(path.read_text())
    if document.get("schema_version") != 1:
        raise FormatError(f"unsupported semantics schema in {path}")
    result: dict[int, dict[str, object]] = {}
    for key, value in document.get("commands", {}).items():
        command_id = int(key, 0)
        if not isinstance(value, dict):
            raise FormatError(f"command {key} in {path} is not an object")
        result[command_id] = value
    return result


def build_database(
    executable_path: Path,
    map_path: Path,
    corpus_paths: tuple[Path, ...],
    semantics_path: Path | None,
    filenames_path: Path,
    table_symbol: str = DEFAULT_TABLE_SYMBOL,
) -> dict[str, object]:
    executable = executable_path.read_bytes()
    symbols_by_name, symbols_by_address = _parse_linker_map(map_path)
    try:
        table_address = symbols_by_name[table_symbol]
    except KeyError as error:
        raise FormatError(f"{table_symbol} is absent from {map_path}") from error
    following_addresses = sorted(
        address for address in symbols_by_address if address > table_address
    )
    if not following_addresses:
        raise FormatError(f"cannot determine the end of {table_symbol}")
    table_end = following_addresses[0]
    if (table_end - table_address) % 4:
        raise FormatError(f"{table_symbol} has a non-word-aligned size")
    command_count = (table_end - table_address) // 4
    handler_data = _read_elf32_virtual(executable, table_address, command_count * 4)
    handlers = struct.unpack(f"<{command_count}I", handler_data)
    symbol_addresses = sorted(symbols_by_address)
    function_ends = dict(zip(symbol_addresses, symbol_addresses[1:]))

    corpus, frequency, examples = (
        _profile_corpus(corpus_paths, filenames_path)
        if corpus_paths
        else ({}, Counter(), {})
    )
    semantics = _load_semantics(semantics_path)
    unknown_overrides = sorted(set(semantics) - set(range(command_count)))
    if unknown_overrides:
        raise FormatError(f"semantics contains out-of-range commands: {unknown_overrides}")

    commands = []
    for command_id, handler_address in enumerate(handlers):
        override = semantics.get(command_id, {})
        inputs = override.get("inputs")
        outputs = override.get("outputs")
        net = override.get("net")
        inferred_inputs = None
        handler_end = function_ends.get(handler_address)
        if handler_end is not None:
            handler_code = _read_elf32_virtual(
                executable, handler_address, handler_end - handler_address
            )
            inferred_inputs = _infer_direct_stack_inputs(handler_code)
        if (
            inputs is None
            and outputs is None
            and net is None
            and inferred_inputs is not None
        ):
            inputs = inferred_inputs
            outputs = 0
            net = -inferred_inputs
            stack_confidence = "high"
            stack_evidence = (
                f"machine-code analysis found a direct Script stackTop decrement of {inferred_inputs}"
                if inferred_inputs
                else "machine-code analysis found a leaf handler with no Script stackTop access"
            )
        else:
            stack_confidence = override.get("stack_confidence", "unknown")
            stack_evidence = override.get("stack_evidence")
        if inputs is not None and outputs is not None:
            calculated_net = int(outputs) - int(inputs)
            if net is not None and net != calculated_net:
                raise FormatError(
                    f"command {command_id}: stack net conflicts with inputs and outputs"
                )
            net = calculated_net
        item: dict[str, object] = {
            "id": command_id,
            "handler_address": f"0x{handler_address:08X}",
            "handler_symbol": _preferred_symbol(symbols_by_address.get(handler_address, ())),
            "name": override.get("name"),
            "name_confidence": override.get("name_confidence", "unknown"),
            "stack": {
                "inputs": inputs,
                "outputs": outputs,
                "net": net,
                "confidence": stack_confidence,
                "evidence": stack_evidence,
            },
            "arguments": override.get("arguments", []),
            "call_count": frequency[command_id],
            "examples": examples.get(command_id, []),
        }
        if override.get("notes"):
            item["notes"] = override["notes"]
        commands.append(item)

    return {
        "schema_version": 1,
        "game_version": "jp",
        "executable_sha1": hashlib.sha1(executable).hexdigest(),
        "command_table": {
            "symbol": table_symbol,
            "address": f"0x{table_address:08X}",
            "end": f"0x{table_end:08X}",
            "count": command_count,
        },
        "corpus": corpus,
        "commands": commands,
    }


def write_database(document: dict[str, object], path: Path) -> None:
    path.write_text(json.dumps(document, indent=2) + "\n")


def _stack_effect(instruction: Instruction, commands: dict[int, CommandMetadata]) -> tuple[int, int] | None:
    opcode = instruction.opcode
    if opcode in (0, 2, 5, 8):
        return 0, 0
    if opcode == 1:
        return (1, 0) if instruction.operand in (5, 15) else (2, -1)
    if opcode in (3, 6, 11, 13, 15, 17, 31):
        return 1, -1
    if opcode in (9, 10, 12, 14, 16, 21, 30):
        return 0, 1
    if opcode in (22, 23, 25):
        return 2, -2
    if opcode == COMMAND_OPCODE:
        command = commands.get(instruction.operand)
        if command is None or command.stack.net is None:
            return None
        minimum = command.stack.inputs if command.stack.inputs is not None else max(0, -command.stack.net)
        return minimum, command.stack.net
    return None


def analyze_stack(blocks: Iterable[ScriptBlock], commands: dict[int, CommandMetadata]) -> dict[str, object]:
    summary = Counter()
    unknown_commands: Counter[int] = Counter()
    issues: list[dict[str, int | str]] = []
    for block in blocks:
        for script in block.scripts:
            base_index = 0
            for entry_index, entry in enumerate(script.entries):
                summary["entries"] += 1
                depths: dict[int, int] = {0: 0}
                pending = [0]
                minimum_depth = 0
                terminal_depths: set[int] = set()
                unresolved = False
                while pending:
                    local_index = pending.pop()
                    if local_index >= len(entry):
                        continue
                    depth = depths[local_index]
                    instruction = entry[local_index]
                    effect = _stack_effect(instruction, commands)
                    if effect is None:
                        unresolved = True
                        if instruction.opcode == COMMAND_OPCODE:
                            unknown_commands[instruction.operand] += 1
                        break
                    minimum, delta = effect
                    minimum_depth = min(minimum_depth, depth - minimum)
                    next_depth = depth + delta
                    if instruction.opcode == 5:
                        terminal_depths.add(next_depth)
                        continue
                    if instruction.opcode in (2, 3):
                        target_global = base_index + local_index + instruction.signed_operand
                        target = target_global - base_index
                        successors = [target]
                        if instruction.opcode == 3:
                            successors.append(local_index + 1)
                    else:
                        successors = [local_index + 1]
                    for successor in successors:
                        if not 0 <= successor < len(entry):
                            issues.append(
                                {
                                    "resource": block.resource_index,
                                    "block": block.block_index,
                                    "script": script.script_index,
                                    "entry": entry_index,
                                    "instruction": base_index + local_index,
                                    "problem": f"control flow exits entry at {base_index + successor}",
                                }
                            )
                            continue
                        previous = depths.get(successor)
                        if previous is None:
                            depths[successor] = next_depth
                            pending.append(successor)
                        elif previous != next_depth:
                            issues.append(
                                {
                                    "resource": block.resource_index,
                                    "block": block.block_index,
                                    "script": script.script_index,
                                    "entry": entry_index,
                                    "instruction": base_index + successor,
                                    "problem": f"stack merge conflict: depths {previous} and {next_depth}",
                                }
                            )
                if unresolved:
                    summary["unresolved_entries"] += 1
                else:
                    summary["resolved_entries"] += 1
                    if len(terminal_depths) > 1:
                        issues.append(
                            {
                                "resource": block.resource_index,
                                "block": block.block_index,
                                "script": script.script_index,
                                "entry": entry_index,
                                "instruction": base_index + len(entry) - 1,
                                "problem": f"END reached at inconsistent stack depths {sorted(terminal_depths)}",
                            }
                        )
                    elif terminal_depths and next(iter(terminal_depths)) != minimum_depth:
                        terminal_depth = next(iter(terminal_depths))
                        issues.append(
                            {
                                "resource": block.resource_index,
                                "block": block.block_index,
                                "script": script.script_index,
                                "entry": entry_index,
                                "instruction": base_index + len(entry) - 1,
                                "problem": (
                                    f"END leaves {terminal_depth - minimum_depth} stack value(s) "
                                    f"after {max(0, -minimum_depth)} inferred input(s)"
                                ),
                            }
                        )
                base_index += len(entry)
    return {
        **dict(summary),
        "issues": issues,
        "unknown_commands": [
            {"id": command_id, "calls": count}
            for command_id, count in unknown_commands.most_common()
        ],
    }


def _generate(args: argparse.Namespace) -> int:
    document = build_database(
        args.executable,
        args.map,
        tuple(args.input),
        args.semantics,
        args.filenames,
        args.table_symbol,
    )
    write_database(document, args.output)
    corpus = document["corpus"]
    print(
        f"wrote {document['command_table']['count']} commands to {args.output}; "
        f"{corpus.get('commands_used', 0)} used commands, {corpus.get('command_calls', 0)} calls",
        file=sys.stderr,
    )
    return 0


def _check(args: argparse.Namespace) -> int:
    commands = load_command_database(args.database)
    aggregate = Counter()
    issues: list[dict[str, int | str]] = []
    unknown: Counter[int] = Counter()
    archives = 0
    for archive_name, data in iter_input_ards(args.input, args.filenames):
        blocks = tuple(find_script_blocks(data))
        if not blocks:
            continue
        archives += 1
        result = analyze_stack(blocks, commands)
        aggregate["entries"] += result["entries"]
        aggregate["resolved_entries"] += result["resolved_entries"]
        aggregate["unresolved_entries"] += result["unresolved_entries"]
        issues.extend(result["issues"])
        for issue in result["issues"]:
            issue["archive"] = archive_name
        unknown.update({item["id"]: item["calls"] for item in result["unknown_commands"]})
    output = {
        "archives": archives,
        **dict(aggregate),
        "issue_count": len(issues),
        "issues": issues,
        "unknown_commands": [
            {"id": command_id, "calls": count} for command_id, count in unknown.most_common()
        ],
    }
    print(json.dumps(output, indent=2))
    return 1 if issues else 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="operation", required=True)

    generate = subparsers.add_parser("generate", help="extract the command table and profile a script corpus")
    generate.add_argument("--executable", type=Path, required=True)
    generate.add_argument("--map", type=Path, required=True)
    generate.add_argument("--input", type=Path, action="append", default=[])
    generate.add_argument("--semantics", type=Path)
    generate.add_argument("--filenames", type=Path, default=DEFAULT_FILENAMES)
    generate.add_argument("--table-symbol", default=DEFAULT_TABLE_SYMBOL)
    generate.add_argument("--output", type=Path, required=True)
    generate.set_defaults(action=_generate)

    check = subparsers.add_parser("check", help="check stack depth where command signatures are known")
    check.add_argument("database", type=Path)
    check.add_argument("input", type=Path, nargs="+")
    check.add_argument("--filenames", type=Path, default=DEFAULT_FILENAMES)
    check.set_defaults(action=_check)

    args = parser.parse_args()
    try:
        return args.action(args)
    except (FormatError, OSError, ValueError, json.JSONDecodeError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
