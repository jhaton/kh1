"""Lossless Kingdom Hearts I area-script bytecode support."""

from __future__ import annotations

import re
import struct
from dataclasses import dataclass
from typing import Iterable, Iterator


OPCODE_NAMES = (
    "NOP",
    "APPLY_OPERATOR",
    "JUMP",
    "JUMP_IF_FALSE",
    "RESERVED_04",
    "END",
    "SAVE_SWITCH_VALUE",
    "SWITCH_EQ",
    "POP_SWITCH_VALUE",
    "PUSH_S24",
    "LOAD_LOCAL",
    "STORE_LOCAL",
    "LOAD_VAR8",
    "STORE_VAR8",
    "LOAD_VAR16",
    "STORE_VAR16",
    "LOAD_VAR32",
    "STORE_VAR32",
    "RESERVED_18",
    "RESERVED_19",
    "RESERVED_20",
    "PUSH_S24_ALT",
    "START_THREAD",
    "WAIT_THREAD",
    "CALL_COMMAND",
    "STOP_THREAD",
    "SWITCH_LT",
    "SWITCH_LE",
    "SWITCH_GT",
    "SWITCH_GE",
    "LOAD_WORK_BIT",
    "STORE_WORK_BIT",
)

OPERATOR_NAMES = (
    "ADD",
    "SUBTRACT",
    "MULTIPLY",
    "DIVIDE",
    "MODULO",
    "NEGATE",
    "EQUAL",
    "GREATER_THAN",
    "GREATER_OR_EQUAL",
    "LESS_THAN",
    "LESS_OR_EQUAL",
    "NOT_EQUAL",
    "BIT_AND",
    "BIT_OR",
    "BIT_XOR",
    "BIT_NOT",
    "SHIFT_RIGHT",
    "SHIFT_LEFT",
)

HEX_OPERAND_OPCODES = frozenset((12, 13, 14, 15, 16, 17, 30, 31))
BRANCH_OPCODES = frozenset((2, 3))
SIGNED_OPERAND_OPCODES = frozenset((2, 3, 9, 21))
OPCODE_BY_NAME = {name: opcode for opcode, name in enumerate(OPCODE_NAMES)}
OPERATOR_BY_NAME = {name: operator for operator, name in enumerate(OPERATOR_NAMES)}
BLOCK_MAGIC = b"KGR\0"
ARCHIVE_ALIGNMENT = 0x80


class FormatError(ValueError):
    pass


@dataclass(frozen=True)
class Instruction:
    opcode: int
    operand: int
    file_offset: int | None = None

    @property
    def signed_operand(self) -> int:
        return decode_s24(self.operand)


@dataclass(frozen=True)
class Script:
    script_index: int
    entries: tuple[tuple[Instruction, ...], ...]

    @property
    def instructions(self) -> tuple[Instruction, ...]:
        return tuple(instruction for entry in self.entries for instruction in entry)


@dataclass(frozen=True)
class ScriptBlock:
    resource_index: int
    block_index: int
    scripts: tuple[Script, ...]


@dataclass(frozen=True)
class ParsedScriptBlock:
    block: ScriptBlock
    start: int
    end: int
    code_end: int
    header: bytes


@dataclass(frozen=True)
class ScriptPackage:
    resource_index: int
    data: bytes
    prefix_count: int
    selected_block: int
    block_count: int
    offsets: tuple[int, ...]
    blocks: tuple[ParsedScriptBlock, ...]


@dataclass(frozen=True)
class ArchiveLayout:
    data: bytes
    offsets: tuple[int, ...]

    @property
    def entry_count(self) -> int:
        return len(self.offsets) - 1

    def entry(self, index: int) -> bytes:
        if not 0 <= index < self.entry_count:
            raise FormatError(f"archive entry index {index} is out of range")
        start = self.offsets[index]
        end = min(self.offsets[index + 1], len(self.data))
        return self.data[start:end]


@dataclass
class _SourceInstruction:
    opcode: int
    operand: str
    line_number: int


@dataclass
class _SourceScript:
    script_index: int
    entries: list[list[_SourceInstruction]]
    labels: dict[str, int]


@dataclass
class _SourceBlock:
    resource_index: int
    block_index: int
    scripts: list[_SourceScript]


def read_u32(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 4 > len(data):
        raise FormatError(f"32-bit read at 0x{offset:X} exceeds a 0x{len(data):X}-byte buffer")
    return struct.unpack_from("<I", data, offset)[0]


def decode_s24(value: int) -> int:
    value &= 0xFFFFFF
    return value - 0x1000000 if value & 0x800000 else value


def encode_s24(value: int) -> int:
    if not -0x800000 <= value <= 0xFFFFFF:
        raise FormatError(f"operand {value} does not fit in 24 bits")
    return value & 0xFFFFFF


def parse_archive(data: bytes) -> ArchiveLayout:
    entry_count = read_u32(data, 0)
    if entry_count > 0x10000:
        raise FormatError(f"implausible archive entry count: {entry_count}")

    table_end = 4 + (entry_count + 1) * 4
    if table_end > len(data):
        raise FormatError("archive offset table is truncated")

    offsets = struct.unpack_from(f"<{entry_count + 1}I", data, 4)
    aligned_size = (len(data) + ARCHIVE_ALIGNMENT - 1) & -ARCHIVE_ALIGNMENT
    previous = table_end
    for index, offset in enumerate(offsets):
        if offset < previous:
            raise FormatError(f"archive offset {index} is not monotonic")
        if offset > aligned_size:
            raise FormatError(f"archive offset 0x{offset:X} exceeds aligned size 0x{aligned_size:X}")
        previous = offset
    return ArchiveLayout(data, offsets)


def _parse_script_block(
    package: bytes,
    resource_index: int,
    block_index: int,
    start: int,
    end: int,
) -> ParsedScriptBlock:
    if start + 13 > end:
        raise FormatError(f"resource {resource_index}, block {block_index}: truncated block header")
    header = package[start : start + 12]
    if header[:4] != BLOCK_MAGIC:
        raise FormatError(
            f"resource {resource_index}, block {block_index}: expected KGR block at 0x{start:X}"
        )

    cursor = start + 12
    script_count = package[cursor]
    cursor += 1
    scripts: list[Script] = []

    for script_index in range(script_count):
        if cursor + 4 > end:
            raise FormatError(
                f"resource {resource_index}, block {block_index}, script {script_index}: truncated header"
            )
        entry_count = package[cursor]
        if package[cursor + 1 : cursor + 4] != b"\0\0\0":
            raise FormatError(
                f"resource {resource_index}, block {block_index}, script {script_index}: "
                "header padding is nonzero"
            )
        cursor += 4
        entries: list[tuple[Instruction, ...]] = []

        for entry_index in range(entry_count):
            instructions: list[Instruction] = []
            while True:
                raw = read_u32(package, cursor)
                opcode = raw >> 24
                instructions.append(Instruction(opcode, raw & 0xFFFFFF, cursor))
                cursor += 4
                if cursor > end:
                    raise FormatError(
                        f"resource {resource_index}, block {block_index}, script {script_index}, "
                        f"entry {entry_index}: bytecode exceeds block boundary"
                    )
                if opcode == 5:
                    break
            entries.append(tuple(instructions))
        scripts.append(Script(script_index, tuple(entries)))

    return ParsedScriptBlock(
        ScriptBlock(resource_index, block_index, tuple(scripts)),
        start,
        end,
        cursor,
        header,
    )


def parse_script_package(package: bytes, resource_index: int) -> ScriptPackage:
    if len(package) < 16:
        raise FormatError(f"resource {resource_index}: script package is truncated")

    prefix_count = read_u32(package, 0)
    selected_block = read_u32(package, 4)
    block_count = read_u32(package, 8)
    if block_count == 0 or block_count > 0x100:
        raise FormatError(f"resource {resource_index}: implausible script block count {block_count}")
    if selected_block >= block_count:
        raise FormatError(
            f"resource {resource_index}: selected block {selected_block} exceeds block count {block_count}"
        )

    offset_count = prefix_count + block_count + 1
    table_end = 12 + offset_count * 4
    if table_end > len(package):
        raise FormatError(f"resource {resource_index}: package offset table is truncated")
    offsets = struct.unpack_from(f"<{offset_count}I", package, 12)
    previous = table_end
    for index, offset in enumerate(offsets):
        if offset < previous or offset > len(package):
            raise FormatError(f"resource {resource_index}: invalid package offset {index}: 0x{offset:X}")
        previous = offset

    script_offsets = offsets[prefix_count + 1 :]
    blocks: list[ParsedScriptBlock] = []
    for block_index, start in enumerate(script_offsets):
        end = script_offsets[block_index + 1] if block_index + 1 < block_count else len(package)
        blocks.append(_parse_script_block(package, resource_index, block_index, start, end))

    return ScriptPackage(
        resource_index,
        package,
        prefix_count,
        selected_block,
        block_count,
        offsets,
        tuple(blocks),
    )


def find_script_packages(ard_data: bytes) -> Iterator[ScriptPackage]:
    outer = parse_archive(ard_data)
    for resource_index in range(outer.entry_count):
        resource = outer.entry(resource_index)
        if not resource:
            continue
        try:
            nested = parse_archive(resource)
        except FormatError:
            continue
        if nested.entry_count != 5:
            continue
        package = nested.entry(3)
        if not package:
            continue
        try:
            yield parse_script_package(package, resource_index)
        except FormatError:
            continue


def find_script_blocks(ard_data: bytes) -> Iterator[ScriptBlock]:
    for package in find_script_packages(ard_data):
        for parsed_block in package.blocks:
            yield parsed_block.block


def find_scripts(ard_data: bytes) -> Iterator[tuple[int, int, Script]]:
    for block in find_script_blocks(ard_data):
        for script in block.scripts:
            yield block.resource_index, block.block_index, script


def opcode_name(opcode: int) -> str:
    if 0 <= opcode < len(OPCODE_NAMES):
        return OPCODE_NAMES[opcode]
    return f"OP_{opcode:02X}"


def _format_source_operand(instruction: Instruction, instruction_index: int, labels: set[int]) -> str:
    opcode = instruction.opcode
    if opcode == 1 and instruction.operand < len(OPERATOR_NAMES):
        return OPERATOR_NAMES[instruction.operand]
    if opcode in BRANCH_OPCODES:
        target = instruction_index + instruction.signed_operand
        if target in labels:
            return f"L{target:04d}"
        return str(instruction.signed_operand)
    if opcode in HEX_OPERAND_OPCODES:
        return f"0x{instruction.operand:06X}"
    if opcode in SIGNED_OPERAND_OPCODES:
        return str(instruction.signed_operand)
    return str(instruction.operand)


def dump_khsasm(blocks: Iterable[ScriptBlock]) -> str:
    lines = [".khsasm 1"]
    previous_resource: int | None = None

    for block in blocks:
        if block.resource_index != previous_resource:
            lines.extend(("", f".resource {block.resource_index}"))
            previous_resource = block.resource_index
        lines.extend((f".block {block.block_index}",))

        for script in block.scripts:
            lines.extend((f".script {script.script_index}",))
            instructions = script.instructions
            labels = {
                index + instruction.signed_operand
                for index, instruction in enumerate(instructions)
                if instruction.opcode in BRANCH_OPCODES
                and 0 <= index + instruction.signed_operand < len(instructions)
            }
            instruction_index = 0
            for entry_index, entry in enumerate(script.entries):
                lines.append(f".entry {entry_index}")
                for instruction in entry:
                    if instruction_index in labels:
                        lines.append(f"L{instruction_index:04d}:")
                    operand = _format_source_operand(instruction, instruction_index, labels)
                    lines.append(f"    {opcode_name(instruction.opcode)} {operand}")
                    instruction_index += 1
        lines.append("")

    return "\n".join(lines).rstrip() + "\n"


def _parse_opcode(token: str, line_number: int) -> int:
    opcode = OPCODE_BY_NAME.get(token.upper())
    if opcode is not None:
        return opcode
    match = re.fullmatch(r"OP_([0-9A-Fa-f]{2})", token)
    if match:
        return int(match.group(1), 16)
    raise FormatError(f"line {line_number}: unknown opcode {token!r}")


def _parse_integer(token: str, line_number: int) -> int:
    try:
        return int(token, 0)
    except ValueError as error:
        raise FormatError(f"line {line_number}: expected integer operand, got {token!r}") from error


def _resolve_source_script(source: _SourceScript) -> Script:
    entries: list[tuple[Instruction, ...]] = []
    instruction_index = 0
    for source_entry in source.entries:
        instructions: list[Instruction] = []
        for item in source_entry:
            if item.opcode == 1 and item.operand.upper() in OPERATOR_BY_NAME:
                operand = OPERATOR_BY_NAME[item.operand.upper()]
            elif item.opcode in BRANCH_OPCODES and item.operand in source.labels:
                operand = source.labels[item.operand] - instruction_index
            else:
                operand = _parse_integer(item.operand, item.line_number)
            instructions.append(Instruction(item.opcode, encode_s24(operand)))
            instruction_index += 1
        if not instructions or instructions[-1].opcode != 5:
            raise FormatError(
                f"script {source.script_index}, entry {len(entries)}: every entry must end with END"
            )
        entries.append(tuple(instructions))
    return Script(source.script_index, tuple(entries))


def parse_khsasm(source: str) -> tuple[ScriptBlock, ...]:
    version_seen = False
    resource_index: int | None = None
    current_block: _SourceBlock | None = None
    current_script: _SourceScript | None = None
    current_entry: list[_SourceInstruction] | None = None
    blocks: list[_SourceBlock] = []
    instruction_index = 0

    for line_number, original_line in enumerate(source.splitlines(), 1):
        line = original_line.split("#", 1)[0].strip()
        if not line:
            continue
        if line.startswith("."):
            parts = line.split()
            if len(parts) != 2:
                raise FormatError(f"line {line_number}: malformed directive")
            directive, value = parts
            if directive == ".khsasm":
                if version_seen or value != "1" or blocks or resource_index is not None:
                    raise FormatError(f"line {line_number}: expected one leading '.khsasm 1'")
                version_seen = True
            elif directive == ".resource":
                if not version_seen:
                    raise FormatError(f"line {line_number}: missing '.khsasm 1' header")
                resource_index = _parse_integer(value, line_number)
                current_block = None
                current_script = None
                current_entry = None
            elif directive == ".block":
                if resource_index is None:
                    raise FormatError(f"line {line_number}: .block requires .resource")
                block_index = _parse_integer(value, line_number)
                current_block = _SourceBlock(resource_index, block_index, [])
                blocks.append(current_block)
                current_script = None
                current_entry = None
            elif directive == ".script":
                if current_block is None:
                    raise FormatError(f"line {line_number}: .script requires .block")
                script_index = _parse_integer(value, line_number)
                if script_index != len(current_block.scripts):
                    raise FormatError(
                        f"line {line_number}: expected script index {len(current_block.scripts)}, got {script_index}"
                    )
                current_script = _SourceScript(script_index, [], {})
                current_block.scripts.append(current_script)
                current_entry = None
                instruction_index = 0
            elif directive == ".entry":
                if current_script is None:
                    raise FormatError(f"line {line_number}: .entry requires .script")
                entry_index = _parse_integer(value, line_number)
                if entry_index != len(current_script.entries):
                    raise FormatError(
                        f"line {line_number}: expected entry index {len(current_script.entries)}, got {entry_index}"
                    )
                current_entry = []
                current_script.entries.append(current_entry)
            else:
                raise FormatError(f"line {line_number}: unknown directive {directive!r}")
            continue

        if line.endswith(":"):
            if current_script is None or current_entry is None:
                raise FormatError(f"line {line_number}: label requires .entry")
            label = line[:-1]
            if not re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", label):
                raise FormatError(f"line {line_number}: invalid label {label!r}")
            if label in current_script.labels:
                raise FormatError(f"line {line_number}: duplicate label {label!r}")
            current_script.labels[label] = instruction_index
            continue

        if current_entry is None:
            raise FormatError(f"line {line_number}: instruction requires .entry")
        parts = line.split()
        if len(parts) != 2:
            raise FormatError(f"line {line_number}: instructions require exactly one operand")
        current_entry.append(_SourceInstruction(_parse_opcode(parts[0], line_number), parts[1], line_number))
        instruction_index += 1

    if not version_seen:
        raise FormatError("missing '.khsasm 1' header")
    if not blocks:
        raise FormatError("assembly contains no script blocks")

    seen_blocks: set[tuple[int, int]] = set()
    resolved: list[ScriptBlock] = []
    for block in blocks:
        key = (block.resource_index, block.block_index)
        if key in seen_blocks:
            raise FormatError(f"duplicate resource {key[0]}, block {key[1]}")
        seen_blocks.add(key)
        if not block.scripts:
            raise FormatError(f"resource {key[0]}, block {key[1]} contains no scripts")
        resolved.append(
            ScriptBlock(
                block.resource_index,
                block.block_index,
                tuple(_resolve_source_script(script) for script in block.scripts),
            )
        )
    return tuple(resolved)


def _encode_script_block(block: ScriptBlock, base: ParsedScriptBlock) -> bytes:
    if len(block.scripts) > 0xFF:
        raise FormatError(f"resource {block.resource_index}, block {block.block_index}: too many scripts")
    output = bytearray(base.header)
    output.append(len(block.scripts))
    for expected_script_index, script in enumerate(block.scripts):
        if script.script_index != expected_script_index:
            raise FormatError(
                f"resource {block.resource_index}, block {block.block_index}: "
                f"expected script index {expected_script_index}, got {script.script_index}"
            )
        if len(script.entries) > 0xFF:
            raise FormatError(
                f"resource {block.resource_index}, block {block.block_index}, "
                f"script {script.script_index}: too many entries"
            )
        output.extend((len(script.entries), 0, 0, 0))
        for entry_index, entry in enumerate(script.entries):
            if not entry or entry[-1].opcode != 5:
                raise FormatError(
                    f"resource {block.resource_index}, block {block.block_index}, "
                    f"script {script.script_index}, entry {entry_index}: missing END"
                )
            for instruction in entry:
                if not 0 <= instruction.opcode <= 0xFF:
                    raise FormatError(f"opcode {instruction.opcode} does not fit in 8 bits")
                output.extend(struct.pack("<I", instruction.opcode << 24 | instruction.operand))
    return bytes(output)


def _align(data: bytearray, alignment: int = ARCHIVE_ALIGNMENT) -> None:
    data.extend(b"\0" * (-len(data) % alignment))


def _rebuild_script_package(package: ScriptPackage, replacements: dict[int, ScriptBlock]) -> bytes:
    unknown_blocks = set(replacements) - set(range(package.block_count))
    if unknown_blocks:
        index = min(unknown_blocks)
        raise FormatError(f"resource {package.resource_index}: block {index} does not exist in base archive")

    first_block_start = package.blocks[0].start
    output = bytearray(package.data[:first_block_start])
    new_offsets: list[int] = []
    for parsed_block in package.blocks:
        if parsed_block.code_end != parsed_block.end and parsed_block.block.block_index + 1 < package.block_count:
            gap = package.data[parsed_block.code_end : parsed_block.end]
            if gap:
                raise FormatError(
                    f"resource {package.resource_index}, block {parsed_block.block.block_index}: "
                    "unexpected bytes between script blocks"
                )
        new_offsets.append(len(output))
        replacement = replacements.get(parsed_block.block.block_index, parsed_block.block)
        output.extend(_encode_script_block(replacement, parsed_block))

    trailing = package.data[package.blocks[-1].code_end : package.blocks[-1].end]
    if any(trailing):
        raise FormatError(
            f"resource {package.resource_index}: nonzero data follows the final script block"
        )
    _align(output)

    for block_index, offset in enumerate(new_offsets):
        offset_index = package.prefix_count + 1 + block_index
        struct.pack_into("<I", output, 12 + offset_index * 4, offset)
    return bytes(output)


def _replace_archive_entry(data: bytes, index: int, replacement: bytes) -> bytes:
    archive = parse_archive(data)
    start = archive.offsets[index]
    physical_end = min(archive.offsets[index + 1], len(data))
    delta = len(replacement) - (physical_end - start)
    output = bytearray(data[:start])
    output.extend(replacement)
    output.extend(data[physical_end:])
    if delta:
        for offset_index in range(index + 1, len(archive.offsets)):
            struct.pack_into("<I", output, 4 + offset_index * 4, archive.offsets[offset_index] + delta)
    return bytes(output)


def assemble_archive(base_ard: bytes, blocks: Iterable[ScriptBlock]) -> bytes:
    replacements_by_resource: dict[int, dict[int, ScriptBlock]] = {}
    for block in blocks:
        resource_blocks = replacements_by_resource.setdefault(block.resource_index, {})
        if block.block_index in resource_blocks:
            raise FormatError(
                f"duplicate replacement for resource {block.resource_index}, block {block.block_index}"
            )
        resource_blocks[block.block_index] = block

    output = base_ard
    for resource_index, replacements in sorted(replacements_by_resource.items()):
        outer = parse_archive(output)
        if not 0 <= resource_index < outer.entry_count:
            raise FormatError(f"resource index {resource_index} does not exist in base archive")
        resource = outer.entry(resource_index)
        nested = parse_archive(resource)
        if nested.entry_count != 5:
            raise FormatError(f"resource {resource_index} is not a script-bearing area resource")
        package_data = nested.entry(3)
        package = parse_script_package(package_data, resource_index)
        rebuilt_package = _rebuild_script_package(package, replacements)
        rebuilt_resource = _replace_archive_entry(resource, 3, rebuilt_package)
        output = _replace_archive_entry(output, resource_index, rebuilt_resource)
    return output
