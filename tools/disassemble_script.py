#!/usr/bin/env python3
"""Disassemble Kingdom Hearts I area-script bytecode from an extracted .ard file."""

from __future__ import annotations

import argparse
import sys
from collections import deque
from pathlib import Path

if __package__:
    from .script_bytecode import (
        BRANCH_OPCODES,
        HEX_OPERAND_OPCODES,
        OPERATOR_NAMES,
        SIGNED_OPERAND_OPCODES,
        FormatError,
        Instruction,
        ScriptBlock,
        dump_khsasm,
        find_script_blocks,
        opcode_name,
    )
    from .script_command_db import CommandMetadata, load_command_database
else:
    from script_bytecode import (
        BRANCH_OPCODES,
        HEX_OPERAND_OPCODES,
        OPERATOR_NAMES,
        SIGNED_OPERAND_OPCODES,
        FormatError,
        Instruction,
        ScriptBlock,
        dump_khsasm,
        find_script_blocks,
        opcode_name,
    )
    from script_command_db import CommandMetadata, load_command_database


def format_operand(instruction: Instruction, instruction_index: int) -> str:
    opcode = instruction.opcode
    if opcode == 1 and instruction.operand < len(OPERATOR_NAMES):
        return OPERATOR_NAMES[instruction.operand]
    if opcode in BRANCH_OPCODES:
        operand = instruction.signed_operand
        return f"{operand:+d} -> {instruction_index + operand:04d}"
    if opcode in HEX_OPERAND_OPCODES:
        return f"0x{instruction.operand:06X}"
    if opcode in SIGNED_OPERAND_OPCODES:
        return str(instruction.signed_operand)
    return str(instruction.operand)


OPERATOR_SYMBOLS = (
    "+",
    "-",
    "*",
    "/",
    "%",
    "-",
    "==",
    ">",
    ">=",
    "<",
    "<=",
    "!=",
    "&",
    "|",
    "^",
    "~",
    ">>",
    "<<",
)


def _pop_values(stack: tuple[str, ...], count: int) -> tuple[tuple[str, ...], tuple[str, ...]] | None:
    if len(stack) < count:
        return None
    return stack[:-count] if count else stack, stack[-count:] if count else ()


def _transfer_symbolic(
    instruction: Instruction,
    stack: tuple[str, ...] | None,
    commands: dict[int, CommandMetadata],
) -> tuple[tuple[str, ...] | None, str | None]:
    opcode = instruction.opcode
    if opcode == 24:
        command = commands.get(instruction.operand)
        if command is None:
            return None, f"command_{instruction.operand}(?) [metadata missing]"
        handler = command.handler_symbol or f"0x{command.handler_address:08X}"
        signature = command.stack
        details = f"id={command.command_id}, handler={handler}, stack "
        details += (
            f"{signature.inputs}->{signature.outputs}"
            if signature.inputs is not None and signature.outputs is not None
            else "unknown"
        )
        if stack is None or signature.inputs is None or signature.outputs is None:
            return None, f"{command.display_name}(?) [{details}]"
        popped = _pop_values(stack, signature.inputs)
        if popped is None:
            return None, f"{command.display_name}(?) [{details}; underflow]"
        remainder, arguments = popped
        call = f"{command.display_name}({', '.join(arguments)})"
        if signature.outputs == 1:
            result = remainder + (call,)
        elif signature.outputs > 1:
            result = remainder + tuple(f"{call}[{index}]" for index in range(signature.outputs))
        else:
            result = remainder
        return result, f"{call} [{details}]"

    if stack is None:
        return None, None
    if opcode in (0, 2, 5, 7, 8, 26, 27, 28, 29):
        return stack, None
    if opcode == 1:
        symbol = OPERATOR_SYMBOLS[instruction.operand] if instruction.operand < len(OPERATOR_SYMBOLS) else "?"
        arity = 1 if instruction.operand in (5, 15) else 2
        popped = _pop_values(stack, arity)
        if popped is None:
            return None, None
        remainder, values = popped
        expression = (
            f"({symbol}{values[0]})"
            if arity == 1
            else f"({values[0]} {symbol} {values[1]})"
        )
        return remainder + (expression,), None
    if opcode == 3:
        popped = _pop_values(stack, 1)
        return (popped[0], None) if popped is not None else (None, None)
    if opcode == 6:
        popped = _pop_values(stack, 1)
        return (popped[0], None) if popped is not None else (None, None)
    if opcode in (9, 21):
        return stack + (str(instruction.signed_operand),), None
    if opcode == 10:
        return stack + (f"local[{instruction.operand}]",), None
    if opcode in (12, 14, 16):
        width = {12: 8, 14: 16, 16: 32}[opcode]
        return stack + (f"var{width}[0x{instruction.operand:06X}]",), None
    if opcode == 30:
        return stack + (f"work_bit[0x{instruction.operand:06X}]",), None
    if opcode in (11, 13, 15, 17, 31):
        popped = _pop_values(stack, 1)
        return (popped[0], None) if popped is not None else (None, None)
    if opcode in (22, 23, 25):
        popped = _pop_values(stack, 2)
        return (popped[0], None) if popped is not None else (None, None)
    return None, None


def _merge_stacks(
    current: tuple[str, ...] | None,
    incoming: tuple[str, ...] | None,
) -> tuple[str, ...] | None:
    if current is None or incoming is None or len(current) != len(incoming):
        return None
    return tuple(left if left == right else "?" for left, right in zip(current, incoming))


def command_annotations(
    entry: tuple[Instruction, ...],
    base_index: int,
    commands: dict[int, CommandMetadata],
) -> dict[int, str]:
    states: dict[int, tuple[str, ...] | None] = {0: ()}
    pending = deque((0,))
    annotations: dict[int, str] = {}
    while pending:
        local_index = pending.popleft()
        instruction = entry[local_index]
        next_stack, annotation = _transfer_symbolic(instruction, states[local_index], commands)
        if annotation is not None:
            previous_annotation = annotations.get(local_index)
            annotations[local_index] = (
                annotation if previous_annotation in (None, annotation) else annotation.split(" [", 1)[0] + "(?)"
            )
        if instruction.opcode == 5:
            continue
        if instruction.opcode in BRANCH_OPCODES:
            target = local_index + instruction.signed_operand
            successors = [target]
            if instruction.opcode == 3:
                successors.append(local_index + 1)
        else:
            successors = [local_index + 1]
        for successor in successors:
            if not 0 <= successor < len(entry):
                continue
            if successor not in states:
                states[successor] = next_stack
                pending.append(successor)
                continue
            merged = _merge_stacks(states[successor], next_stack)
            if merged != states[successor]:
                states[successor] = merged
                pending.append(successor)
    return annotations


def print_disassembly(
    blocks: tuple[ScriptBlock, ...],
    resource_filter: int | None,
    script_filter: int | None,
    commands: dict[int, CommandMetadata] | None,
) -> tuple[int, int]:
    script_count = 0
    instruction_count = 0
    for block in blocks:
        if resource_filter is not None and block.resource_index != resource_filter:
            continue
        for script in block.scripts:
            if script_filter is not None and script.script_index != script_filter:
                continue
            script_count += 1
            print(
                f"resource {block.resource_index:02d}, block {block.block_index:02d}, "
                f"script {script.script_index:02d}: "
                f"{len(script.entries)} entr{'y' if len(script.entries) == 1 else 'ies'}"
            )
            instruction_index = 0
            for entry_index, entry in enumerate(script.entries):
                annotations = (
                    command_annotations(entry, instruction_index, commands)
                    if commands is not None
                    else {}
                )
                entry_start = instruction_index
                print(f"  entry {entry_index:02d}:")
                for instruction in entry:
                    instruction_count += 1
                    operand = format_operand(instruction, instruction_index)
                    offset = instruction.file_offset
                    offset_text = f"@{offset:06X}" if offset is not None else "@??????"
                    annotation = annotations.get(instruction_index - entry_start)
                    annotation_text = f"  ; {annotation}" if annotation is not None else ""
                    print(
                        f"    {instruction_index:04d}  {offset_text}  "
                        f"{opcode_name(instruction.opcode)} {operand}{annotation_text}"
                    )
                    instruction_index += 1
    return script_count, instruction_count


def disassemble(
    path: Path,
    resource_filter: int | None,
    script_filter: int | None,
    output: Path | None,
    command_database: Path | None,
) -> int:
    blocks = tuple(find_script_blocks(path.read_bytes()))
    commands = load_command_database(command_database) if command_database is not None else None
    if not blocks:
        raise FormatError("no script resources found")

    if output is not None:
        if resource_filter is not None or script_filter is not None:
            raise FormatError("--output cannot be combined with --resource or --script")
        command_names = (
            {command_id: metadata.display_name for command_id, metadata in commands.items()}
            if commands is not None
            else None
        )
        output.write_text(dump_khsasm(blocks, command_names))
        script_count = sum(len(block.scripts) for block in blocks)
        instruction_count = sum(
            len(entry)
            for block in blocks
            for script in block.scripts
            for entry in script.entries
        )
        print(
            f"wrote {len(blocks)} blocks, {script_count} scripts, "
            f"{instruction_count} instructions to {output}",
            file=sys.stderr,
        )
        return 0

    script_count, instruction_count = print_disassembly(blocks, resource_filter, script_filter, commands)
    if script_count == 0:
        raise FormatError("no matching scripts found")
    print(f"\n{script_count} scripts, {instruction_count} instructions", file=sys.stderr)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path, help="decompressed .ard file produced by tools/iso/extract.py")
    parser.add_argument("--resource", type=int, help="show only this outer archive resource index")
    parser.add_argument("--script", type=int, help="show only this script index")
    parser.add_argument("-o", "--output", type=Path, help="write lossless .khsasm source")
    parser.add_argument("--command-db", type=Path, help="annotate calls using this command metadata database")
    args = parser.parse_args()

    try:
        return disassemble(args.archive, args.resource, args.script, args.output, args.command_db)
    except (FormatError, OSError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
