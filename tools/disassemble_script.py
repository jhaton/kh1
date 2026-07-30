#!/usr/bin/env python3
"""Disassemble Kingdom Hearts I area-script bytecode from an extracted .ard file."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path

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


def print_disassembly(
    blocks: tuple[ScriptBlock, ...],
    resource_filter: int | None,
    script_filter: int | None,
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
                print(f"  entry {entry_index:02d}:")
                for instruction in entry:
                    instruction_count += 1
                    operand = format_operand(instruction, instruction_index)
                    offset = instruction.file_offset
                    offset_text = f"@{offset:06X}" if offset is not None else "@??????"
                    print(
                        f"    {instruction_index:04d}  {offset_text}  "
                        f"{opcode_name(instruction.opcode)} {operand}"
                    )
                    instruction_index += 1
    return script_count, instruction_count


def disassemble(
    path: Path,
    resource_filter: int | None,
    script_filter: int | None,
    output: Path | None,
) -> int:
    blocks = tuple(find_script_blocks(path.read_bytes()))
    if not blocks:
        raise FormatError("no script resources found")

    if output is not None:
        if resource_filter is not None or script_filter is not None:
            raise FormatError("--output cannot be combined with --resource or --script")
        output.write_text(dump_khsasm(blocks))
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

    script_count, instruction_count = print_disassembly(blocks, resource_filter, script_filter)
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
    args = parser.parse_args()

    try:
        return disassemble(args.archive, args.resource, args.script, args.output)
    except (FormatError, OSError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
