#!/usr/bin/env python3
"""Disassemble Kingdom Hearts I area-script bytecode from an extracted .ard file."""

from __future__ import annotations

import argparse
import struct
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Iterator


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
NO_OPERAND_OPCODES = frozenset((0, 4, 5, 8, 18, 19, 20, 23, 25))


class FormatError(ValueError):
    pass


@dataclass(frozen=True)
class Instruction:
    index: int
    file_offset: int
    opcode: int
    operand: int


@dataclass(frozen=True)
class Script:
    resource_index: int
    script_index: int
    entries: tuple[tuple[Instruction, ...], ...]


def read_u32(data: bytes, offset: int) -> int:
    if offset < 0 or offset + 4 > len(data):
        raise FormatError(f"32-bit read at 0x{offset:X} exceeds a 0x{len(data):X}-byte resource")
    return struct.unpack_from("<I", data, offset)[0]


def parse_archive(data: bytes) -> tuple[bytes, ...]:
    """Parse the aligned count-and-offset archive used recursively by KH1 area files."""
    count = read_u32(data, 0)
    if count > 0x10000:
        raise FormatError(f"implausible archive entry count: {count}")

    table_end = 4 + (count + 1) * 4
    if table_end > len(data):
        raise FormatError("archive offset table is truncated")

    offsets = struct.unpack_from(f"<{count + 1}I", data, 4)
    aligned_size = (len(data) + 0x7F) & ~0x7F
    if offsets and offsets[-1] > aligned_size:
        raise FormatError(f"archive end 0x{offsets[-1]:X} exceeds aligned size 0x{aligned_size:X}")

    entries: list[bytes] = []
    previous = table_end
    for index, (start, end) in enumerate(zip(offsets, offsets[1:])):
        if start < previous or end < start:
            raise FormatError(f"archive entry {index} has non-monotonic offsets")
        if start > len(data):
            raise FormatError(f"archive entry {index} starts beyond end of file")
        entries.append(data[start : min(end, len(data))])
        previous = start
    return tuple(entries)


def decode_s24(value: int) -> int:
    return value - 0x1000000 if value & 0x800000 else value


def parse_script_package(package: bytes, resource_index: int) -> tuple[Script, ...]:
    """Locate and decode the selected script block in an area script package."""
    table_prefix_count = read_u32(package, 0)
    selected_block = read_u32(package, 4)
    selected_offset_index = table_prefix_count + selected_block + 1
    selected_offset = read_u32(package, 0x0C + selected_offset_index * 4)
    cursor = selected_offset + 0x0C
    if cursor >= len(package):
        raise FormatError("selected script block starts beyond package end")

    script_count = package[cursor]
    cursor += 1
    scripts: list[Script] = []

    for script_index in range(script_count):
        if cursor + 4 > len(package):
            raise FormatError(f"script {script_index} header is truncated")
        entry_count = package[cursor]
        if package[cursor + 1 : cursor + 4] != b"\0\0\0":
            raise FormatError(f"script {script_index} header padding is nonzero")
        cursor += 4

        instruction_index = 0
        entries: list[tuple[Instruction, ...]] = []
        for entry_index in range(entry_count):
            instructions: list[Instruction] = []
            while True:
                raw = read_u32(package, cursor)
                opcode = raw >> 24
                if opcode >= len(OPCODE_NAMES):
                    raise FormatError(
                        f"script {script_index}, entry {entry_index}: invalid opcode {opcode} at 0x{cursor:X}"
                    )
                instruction = Instruction(
                    index=instruction_index,
                    file_offset=cursor,
                    opcode=opcode,
                    operand=decode_s24(raw & 0xFFFFFF),
                )
                instructions.append(instruction)
                instruction_index += 1
                cursor += 4
                if opcode == 5:
                    break
            entries.append(tuple(instructions))

        scripts.append(Script(resource_index, script_index, tuple(entries)))

    return tuple(scripts)


def find_scripts(ard_data: bytes) -> Iterator[Script]:
    """Walk an area archive and yield scripts from each populated script resource."""
    for resource_index, resource in enumerate(parse_archive(ard_data)):
        if not resource:
            continue
        try:
            nested_entries = parse_archive(resource)
        except FormatError:
            continue
        if len(nested_entries) != 5 or not nested_entries[3]:
            continue
        try:
            yield from parse_script_package(nested_entries[3], resource_index)
        except FormatError:
            continue


def format_operand(instruction: Instruction) -> str:
    opcode = instruction.opcode
    operand = instruction.operand
    if opcode in NO_OPERAND_OPCODES:
        return ""
    if opcode == 1:
        if 0 <= operand < len(OPERATOR_NAMES):
            return OPERATOR_NAMES[operand]
        return f"OPERATOR_{operand}"
    if opcode in (2, 3):
        return f"{operand:+d} -> {instruction.index + operand:04d}"
    if opcode in HEX_OPERAND_OPCODES:
        return f"0x{operand & 0xFFFFFF:06X}"
    return str(operand)


def disassemble(path: Path, resource_filter: int | None, script_filter: int | None) -> int:
    scripts = tuple(
        script
        for script in find_scripts(path.read_bytes())
        if (resource_filter is None or script.resource_index == resource_filter)
        and (script_filter is None or script.script_index == script_filter)
    )
    if not scripts:
        raise FormatError("no matching script resources found")

    instruction_count = 0
    for script in scripts:
        print(
            f"resource {script.resource_index:02d}, script {script.script_index:02d}: "
            f"{len(script.entries)} entr{'y' if len(script.entries) == 1 else 'ies'}"
        )
        for entry_index, entry in enumerate(script.entries):
            print(f"  entry {entry_index:02d}:")
            for instruction in entry:
                instruction_count += 1
                operand = format_operand(instruction)
                suffix = f" {operand}" if operand else ""
                print(
                    f"    {instruction.index:04d}  @{instruction.file_offset:06X}  "
                    f"{OPCODE_NAMES[instruction.opcode]}{suffix}"
                )

    print(f"\n{len(scripts)} scripts, {instruction_count} instructions", file=sys.stderr)
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("archive", type=Path, help="decompressed .ard file produced by tools/iso/extract.py")
    parser.add_argument("--resource", type=int, help="show only this outer archive resource index")
    parser.add_argument("--script", type=int, help="show only this script index")
    args = parser.parse_args()

    try:
        return disassemble(args.archive, args.resource, args.script)
    except (FormatError, OSError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
