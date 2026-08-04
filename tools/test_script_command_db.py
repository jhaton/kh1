import json
import tempfile
import unittest
from pathlib import Path

from tools.script_bytecode import FormatError
from tools.script_command_db import (
    _load_semantic_records,
    _load_semantics,
    _require_reviewed_used_commands,
    build_command_symbol_manifest,
    propagate_command_symbols,
    write_command_symbol_manifest,
    write_command_evidence,
)


class SemanticRecordTests(unittest.TestCase):
    def setUp(self) -> None:
        self.temporary_directory = tempfile.TemporaryDirectory()
        self.root = Path(self.temporary_directory.name)

    def tearDown(self) -> None:
        self.temporary_directory.cleanup()

    def write_record(self, command_id: int, record: dict, filename: str | None = None) -> Path:
        path = self.root / (filename or f"{command_id:03d}.json")
        path.write_text(json.dumps(record))
        return path

    def named_record(self, command_id: int, name: str) -> dict:
        return {
            "schema_version": 1,
            "id": command_id,
            "status": "named",
            "name": name,
            "name_confidence": "high",
            "stack": {
                "inputs": 1,
                "outputs": 0,
                "confidence": "high",
                "evidence": "The native handler consumes one stack value.",
            },
            "arguments": [{"name": "value", "type": "s32"}],
        }

    def test_loads_independent_named_and_unresolved_records(self) -> None:
        self.write_record(7, self.named_record(7, "set_example_value"))
        self.write_record(
            31,
            {
                "schema_version": 1,
                "id": 31,
                "status": "reviewed_unresolved",
                "review": {"reason": "The observed state bit has no identified consumer."},
            },
        )

        records = _load_semantic_records(self.root)
        semantics = _load_semantics(self.root)

        self.assertEqual(set(records), {7, 31})
        self.assertEqual(
            semantics[7],
            {
                "name": "set_example_value",
                "name_confidence": "high",
                "arguments": [{"name": "value", "type": "s32"}],
                "inputs": 1,
                "outputs": 0,
                "stack_confidence": "high",
                "stack_evidence": "The native handler consumes one stack value.",
            },
        )
        self.assertEqual(semantics[31], {})

    def test_rejects_filename_and_command_id_mismatch(self) -> None:
        self.write_record(7, self.named_record(7, "set_example_value"), "008.json")

        with self.assertRaisesRegex(FormatError, "filename does not match"):
            _load_semantic_records(self.root)

    def test_rejects_nested_record(self) -> None:
        nested = self.root / "nested"
        nested.mkdir()
        (nested / "007.json").write_text(json.dumps(self.named_record(7, "set_example_value")))

        with self.assertRaisesRegex(FormatError, "stored directly"):
            _load_semantic_records(self.root)

    def test_rejects_duplicate_name_without_explicit_relation(self) -> None:
        self.write_record(7, self.named_record(7, "set_example_value"))
        self.write_record(8, self.named_record(8, "set_example_value"))

        with self.assertRaisesRegex(FormatError, "duplicate semantic name"):
            _load_semantic_records(self.root)

    def test_accepts_reciprocal_variant_relation(self) -> None:
        first = self.named_record(7, "set_example_value")
        second = self.named_record(8, "set_example_value")
        first["relations"] = [
            {"command_id": 8, "kind": "variant", "difference": "Uses storage A."}
        ]
        second["relations"] = [
            {"command_id": 7, "kind": "variant", "difference": "Uses storage B."}
        ]
        self.write_record(7, first)
        self.write_record(8, second)

        self.assertEqual(set(_load_semantic_records(self.root)), {7, 8})

    def test_rejects_argument_count_that_disagrees_with_stack(self) -> None:
        record = self.named_record(7, "set_example_value")
        record["arguments"] = []
        self.write_record(7, record)

        with self.assertRaisesRegex(FormatError, "argument count"):
            _load_semantic_records(self.root)

    def test_rejects_used_command_without_review_record(self) -> None:
        with self.assertRaisesRegex(FormatError, "used commands.*\\[8\\]"):
            _require_reviewed_used_commands({7, 8}, {7})

    def test_writes_independent_machine_evidence_records(self) -> None:
        records = [
            {"id": 0, "handler_address": "0x00100000"},
            {"id": 101, "handler_address": "0x00100100"},
        ]

        write_command_evidence(records, self.root)

        first = json.loads((self.root / "000.json").read_text())
        second = json.loads((self.root / "101.json").read_text())
        self.assertEqual(first["command"], records[0])
        self.assertEqual(second["command"], records[1])

    def test_builds_collision_free_high_confidence_symbol_manifest(self) -> None:
        database = {
            "game_version": "jp",
            "executable_sha1": "abc",
            "commands": [
                {
                    "id": 7,
                    "handler_address": "0x00100000",
                    "handler_symbol": "func_00100000",
                    "name": "set_example_value",
                    "name_confidence": "high",
                },
                {
                    "id": 8,
                    "handler_address": "0x00100010",
                    "handler_symbol": "func_00100010",
                    "name": "noop",
                    "name_confidence": "high",
                },
                {
                    "id": 9,
                    "handler_address": "0x00100020",
                    "handler_symbol": "func_00100020",
                    "name": "noop",
                    "name_confidence": "high",
                },
                {
                    "id": 10,
                    "handler_address": "0x00100030",
                    "handler_symbol": "ScriptCommand_ExistingName",
                    "name": "different_semantic_name",
                    "name_confidence": "high",
                },
                {
                    "id": 11,
                    "handler_address": "0x00100040",
                    "handler_symbol": "func_00100040",
                    "name": "uncertain_name",
                    "name_confidence": "medium",
                },
            ],
        }

        manifest = build_command_symbol_manifest(
            database,
            {0x00100030: ("ScriptCommand_ExistingName",)},
        )

        self.assertEqual(
            [symbol["symbol"] for symbol in manifest["symbols"]],
            [
                "ScriptCommand_SetExampleValue",
                "ScriptCommand_Noop_Cmd008",
                "ScriptCommand_Noop_Cmd009",
                "ScriptCommand_ExistingName",
            ],
        )

    def test_rejects_symbol_name_owned_by_another_address(self) -> None:
        database = {
            "commands": [
                {
                    "id": 7,
                    "handler_address": "0x00100000",
                    "handler_symbol": "func_00100000",
                    "name": "set_example_value",
                    "name_confidence": "high",
                }
            ]
        }

        with self.assertRaisesRegex(FormatError, "already names"):
            build_command_symbol_manifest(
                database,
                {0x00200000: ("ScriptCommand_SetExampleValue",)},
            )

    def test_writes_pcsx2_symbol_manifest(self) -> None:
        manifest = {
            "symbols": [
                {
                    "address": "0x00100000",
                    "symbol": "ScriptCommand_SetExampleValue",
                }
            ]
        }
        output = self.root / "commands.sym"

        write_command_symbol_manifest(manifest, output, "pcsx2")

        self.assertEqual(
            output.read_text(),
            "00100000 ScriptCommand_SetExampleValue\n",
        )

    def test_writes_splat_symbol_manifest(self) -> None:
        manifest = {
            "symbols": [
                {
                    "address": "0x00100000",
                    "symbol": "ScriptCommand_SetExampleValue",
                }
            ]
        }
        output = self.root / "commands.txt"

        write_command_symbol_manifest(manifest, output, "splat")

        self.assertEqual(
            output.read_text(),
            f"{'ScriptCommand_SetExampleValue':<72} = 0x00100000; // type:func\n",
        )

    def test_propagates_manifest_symbols_as_exact_tokens(self) -> None:
        manifest = {
            "symbols": [
                {
                    "current_symbol": "func_00100000",
                    "symbol": "ScriptCommand_SetExampleValue",
                }
            ]
        }
        source = self.root / "example.c"
        source.write_text(
            "void func_00100000(void);\\n"
            "void call(void) { func_00100000(); }\\n"
            "void func_00100000_extra(void);\\n"
        )

        preview = propagate_command_symbols(manifest, [source])
        self.assertFalse(preview["applied"])
        self.assertEqual(preview["replacements"], 2)
        self.assertIn("func_00100000();", source.read_text())

        result = propagate_command_symbols(manifest, [source], apply=True)
        self.assertTrue(result["applied"])
        self.assertEqual(result["files_changed"], 1)
        self.assertEqual(
            source.read_text(),
            "void ScriptCommand_SetExampleValue(void);\\n"
            "void call(void) { ScriptCommand_SetExampleValue(); }\\n"
            "void func_00100000_extra(void);\\n",
        )

    def test_rejects_manifest_symbol_without_repository_reference(self) -> None:
        manifest = {
            "symbols": [
                {
                    "current_symbol": "func_00100000",
                    "symbol": "ScriptCommand_SetExampleValue",
                }
            ]
        }
        source = self.root / "example.c"
        source.write_text("void unrelated(void);\\n")

        with self.assertRaisesRegex(FormatError, "no repository references"):
            propagate_command_symbols(manifest, [source])


if __name__ == "__main__":
    unittest.main()
