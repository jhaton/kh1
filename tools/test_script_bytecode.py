import struct
import unittest

from tools.disassemble_script import command_annotations
from tools.script_bytecode import (
    Instruction,
    Script,
    ScriptBlock,
    FormatError,
    assemble_archive,
    dump_khsasm,
    find_script_blocks,
    parse_archive,
    parse_khsasm,
    parse_script_package,
)
from tools.script_command_db import (
    CommandMetadata,
    StackSignature,
    _infer_direct_stack_inputs,
    _stack_effect,
    analyze_stack,
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


def build_call_block(*command_ids: int) -> tuple[ScriptBlock, ...]:
    entry = tuple(Instruction(24, command_id) for command_id in command_ids) + (Instruction(5, 16),)
    return (ScriptBlock(0, 0, (Script(0, (entry,)),)),)


def build_call_source(version: int, operand: str) -> str:
    return "\n".join(
        (
            f".khsasm {version}",
            "",
            ".resource 0",
            ".block 0",
            ".script 0",
            ".entry 0",
            f"    CALL_COMMAND {operand}",
            "    END 16",
            "",
        )
    )


class ScriptBytecodeTests(unittest.TestCase):
    def test_disassembly_roundtrips_exactly(self) -> None:
        original = build_area_archive()
        source = dump_khsasm(find_script_blocks(original))
        blocks = parse_khsasm(source)

        self.assertEqual(dump_khsasm(blocks), source)
        self.assertEqual(assemble_archive(original, blocks), original)
        self.assertIn("END 16", source)

    def test_mapped_commands_emit_canonical_v2_and_parse(self) -> None:
        blocks = build_call_block(27)
        command_names = {27: "fade_screen_in"}

        source = dump_khsasm(blocks, command_names)

        self.assertEqual(
            source,
            ".khsasm 2\n"
            "\n"
            ".resource 0\n"
            ".block 0\n"
            ".script 0\n"
            ".entry 0\n"
            "    CALL_COMMAND fade_screen_in@27\n"
            "    END 16\n",
        )
        parsed = parse_khsasm(source, command_names)
        self.assertEqual(parsed[0].scripts[0].entries[0][0], Instruction(24, 27))
        self.assertEqual(parsed, blocks)

    def test_numeric_command_source_remains_v1_compatible(self) -> None:
        blocks = build_call_block(27)

        source = dump_khsasm(blocks)

        self.assertTrue(source.startswith(".khsasm 1\n"))
        self.assertIn("    CALL_COMMAND 27\n", source)
        parsed = parse_khsasm(source)
        self.assertEqual(parsed[0].scripts[0].entries[0][0].operand, 27)
        self.assertEqual(parsed, blocks)

    def test_v2_accepts_numeric_command_operand_without_metadata(self) -> None:
        parsed = parse_khsasm(build_call_source(2, "27"))

        self.assertEqual(parsed[0].scripts[0].entries[0][0], Instruction(24, 27))

    def test_duplicate_command_names_are_disambiguated_by_id(self) -> None:
        blocks = build_call_block(27, 28)
        command_names = {27: "fade_screen", 28: "fade_screen"}

        source = dump_khsasm(blocks, command_names)

        self.assertIn("    CALL_COMMAND fade_screen@27\n", source)
        self.assertIn("    CALL_COMMAND fade_screen@28\n", source)
        parsed = parse_khsasm(source, command_names)
        self.assertEqual(
            tuple(instruction.operand for instruction in parsed[0].scripts[0].entries[0]),
            (27, 28, 16),
        )
        self.assertEqual(parsed, blocks)

    def test_unnamed_command_style_roundtrips_symbolically(self) -> None:
        blocks = build_call_block(31)
        command_names = {31: "command_31"}

        source = dump_khsasm(blocks, command_names)

        self.assertIn("    CALL_COMMAND command_31@31\n", source)
        parsed = parse_khsasm(source, command_names)
        self.assertEqual(parsed[0].scripts[0].entries[0][0].operand, 31)
        self.assertEqual(parsed, blocks)

    def test_mapped_dump_requires_every_command_id(self) -> None:
        with self.assertRaisesRegex(FormatError, "missing command name for ID 31"):
            dump_khsasm(build_call_block(31), {27: "fade_screen_in"})

    def test_symbolic_command_name_must_match_id(self) -> None:
        with self.assertRaises(FormatError) as raised:
            parse_khsasm(build_call_source(2, "wrong_name@31"), {31: "command_31"})

        self.assertEqual(
            str(raised.exception),
            "line 7: command name 'wrong_name' does not match ID 31 (expected 'command_31')",
        )

    def test_symbolic_command_id_must_exist_in_metadata(self) -> None:
        with self.assertRaises(FormatError) as raised:
            parse_khsasm(build_call_source(2, "command_31@31"), {27: "fade_screen_in"})

        self.assertEqual(str(raised.exception), "line 7: unknown command ID 31")

    def test_symbolic_command_requires_metadata(self) -> None:
        with self.assertRaises(FormatError) as raised:
            parse_khsasm(build_call_source(2, "command_31@31"))

        self.assertEqual(
            str(raised.exception),
            "line 7: symbolic CALL_COMMAND operand requires command metadata",
        )

    def test_v1_rejects_symbolic_command_operand(self) -> None:
        with self.assertRaises(FormatError) as raised:
            parse_khsasm(build_call_source(1, "command_31@31"), {31: "command_31"})

        self.assertEqual(
            str(raised.exception),
            "line 7: symbolic CALL_COMMAND operands require '.khsasm 2'",
        )

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

    def test_vm_stack_effects_match_runtime_handlers(self) -> None:
        pushes = (9, 10, 12, 14, 16, 21, 30)
        pops = (3, 6, 11, 13, 15, 17, 31)

        for opcode in pushes:
            self.assertEqual(_stack_effect(Instruction(opcode, 0), {}), (0, 1))
        for opcode in pops:
            self.assertEqual(_stack_effect(Instruction(opcode, 0), {}), (1, -1))
        for opcode in (22, 23, 25):
            self.assertEqual(_stack_effect(Instruction(opcode, 0), {}), (2, -2))
        for opcode in (7, 26, 27, 28, 29):
            self.assertIsNone(_stack_effect(Instruction(opcode, 0), {}))

    def test_binary_analysis_recovers_direct_pop_and_stack_free_leaf(self) -> None:
        load_stack_top = 35 << 26 | 4 << 21 | 2 << 16 | 0x180
        subtract_three = 9 << 26 | 2 << 21 | 2 << 16 | 0xFFFD
        store_stack_top = 43 << 26 | 4 << 21 | 2 << 16 | 0x180
        direct_pop = struct.pack("<3I", load_stack_top, subtract_three, store_stack_top)
        leaf = struct.pack("<2I", 31 << 21 | 8, 0)
        delegated = struct.pack("<I", 3 << 26)

        self.assertEqual(_infer_direct_stack_inputs(direct_pop), 3)
        self.assertEqual(_infer_direct_stack_inputs(leaf), 0)
        self.assertIsNone(_infer_direct_stack_inputs(delegated))

    def test_stack_analysis_infers_entry_arguments(self) -> None:
        entry = (Instruction(11, 0), Instruction(5, 16))
        blocks = (ScriptBlock(0, 0, (Script(0, (entry,)),)),)

        result = analyze_stack(blocks, {})

        self.assertEqual(result["resolved_entries"], 1)
        self.assertEqual(result["unresolved_entries"], 0)
        self.assertEqual(result["issues"], [])

    def test_command_annotation_shows_symbolic_argument(self) -> None:
        command = CommandMetadata(
            27,
            0x001CF788,
            "func_001CF788",
            "fade_screen_in",
            "high",
            StackSignature(1, 0, -1, "high"),
            108,
        )
        entry = (Instruction(9, 30), Instruction(24, 27), Instruction(5, 16))

        annotations = command_annotations(entry, 0, {27: command})

        self.assertEqual(
            annotations[1],
            "fade_screen_in(30) [id=27, handler=func_001CF788, stack 1->0]",
        )


if __name__ == "__main__":
    unittest.main()
