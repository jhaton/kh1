import struct
import unittest

from tools.script_bytecode import (
    assemble_archive,
    dump_khsasm,
    find_script_blocks,
    parse_archive,
    parse_khsasm,
    parse_script_package,
)


def build_archive(entries: list[bytes]) -> bytes:
    table_size = 4 + (len(entries) + 1) * 4
    output = bytearray(table_size)
    struct.pack_into("<I", output, 0, len(entries))
    offsets: list[int] = []
    for entry in entries:
        output.extend(b"\0" * (-len(output) % 0x80))
        offsets.append(len(output))
        output.extend(entry)
    output.extend(b"\0" * (-len(output) % 0x80))
    offsets.append(len(output))
    struct.pack_into(f"<{len(offsets)}I", output, 4, *offsets)
    return bytes(output)


def build_script_block(value: int) -> bytes:
    instructions = (
        9 << 24 | value,
        5 << 24 | 16,
    )
    return b"KGR\0" + struct.pack("<II", 1, 1) + b"\x01\x01\0\0\0" + struct.pack("<II", *instructions)


def build_script_package() -> bytes:
    first_block = build_script_block(7)
    second_block = build_script_block(11)
    table_size = 12 + 3 * 4
    first_offset = 0x80
    second_offset = first_offset + len(first_block)
    output = bytearray(struct.pack("<III", 0, 0, 2))
    output.extend(struct.pack("<III", table_size, first_offset, second_offset))
    output.extend(b"\0" * (first_offset - len(output)))
    output.extend(first_block)
    output.extend(second_block)
    output.extend(b"\0" * (-len(output) % 0x80))
    return bytes(output)


def build_area_archive() -> bytes:
    resource = build_archive((b"layout", b"", b"", build_script_package(), b"tail"))
    return build_archive((resource,))


class ScriptBytecodeTests(unittest.TestCase):
    def test_disassembly_roundtrips_exactly(self) -> None:
        original = build_area_archive()
        source = dump_khsasm(find_script_blocks(original))
        blocks = parse_khsasm(source)

        self.assertEqual(dump_khsasm(blocks), source)
        self.assertEqual(assemble_archive(original, blocks), original)
        self.assertIn("END 16", source)

    def test_resized_block_updates_following_offset(self) -> None:
        original = build_area_archive()
        source = dump_khsasm(find_script_blocks(original))
        inserted_instructions = "    NOP 0\n" * 40
        modified_source = source.replace("    END 16", inserted_instructions + "    END 16", 1)
        rebuilt = assemble_archive(original, parse_khsasm(modified_source))

        original_resource = parse_archive(original).entry(0)
        original_package_data = parse_archive(original_resource).entry(3)
        original_package = parse_script_package(original_package_data, 0)
        rebuilt_resource = parse_archive(rebuilt).entry(0)
        rebuilt_package_data = parse_archive(rebuilt_resource).entry(3)
        rebuilt_package = parse_script_package(rebuilt_package_data, 0)

        self.assertEqual(rebuilt_package.blocks[1].start, original_package.blocks[1].start + 160)
        self.assertGreater(len(rebuilt), len(original))
        instruction_count = sum(
            len(entry)
            for block in find_script_blocks(rebuilt)
            for script in block.scripts
            for entry in script.entries
        )
        self.assertEqual(instruction_count, 44)
        rebuilt_source = dump_khsasm(find_script_blocks(rebuilt))
        self.assertEqual(assemble_archive(rebuilt, parse_khsasm(rebuilt_source)), rebuilt)


if __name__ == "__main__":
    unittest.main()
