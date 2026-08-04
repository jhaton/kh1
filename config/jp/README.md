# JP Script Command Database

This directory contains the recovered metadata for the original Japanese release of Kingdom Hearts.

## Terminology

The area-script virtual machine has a small instruction set. Its instruction names and numeric opcodes are defined by `OPCODE_NAMES` in [`../../tools/script_bytecode.py`](../../tools/script_bytecode.py).

`CALL_COMMAND` is VM opcode 24. Its operand is a **native command ID** that selects one of the 749 C handler functions registered by the JP executable. Numbers commonly described during analysis as “opcode 142” or “opcode 55” are therefore more precisely command IDs 142 and 55.

## Runtime organization and invocation

Area scripts are not attached to arbitrary native objects as components. The runtime is closer to an area-owned bank of event scripts whose current target is identified through the engine's encoded object IDs.

### Storage and loading

An `.ard` can contain multiple script-bearing outer resources. Each such resource is a five-entry nested archive; nested entry 3 contains a KGR script package. A package records one or more alternative blocks, a selected block index, and a sequence of scripts. Each script contains a numbered entry table rather than one undifferentiated instruction stream.

[`../../tools/script_bytecode.py`](../../tools/script_bytecode.py) exposes this hierarchy as `ScriptPackage` -> `ScriptBlock` -> `Script` -> entries. When native function `func_001C5918` loads a block, it creates one approximately `0x350`-byte runtime record per script, including:

- The bytecode address and entry instruction indices
- The current instruction cursor
- The value-stack top and value stack
- The current selected-object ID and resolved native pointer
- Coroutine, wait, and command-specific state

Loading then runs entry 0 synchronously until `END`, changes the cursor to entry 1, and registers the scripts for task-driven execution. Entry 0 therefore serves as initialization, while entry 1 is the persistent coroutine in the recovered path.

### Object selection

Scripts commonly select their target during entry 0:

```text
PUSH_S24 0x030001
CALL_COMMAND 10  ; select_object
END
```

Command 10 writes the operand to `Script` offset `0x00`, resolves the encoded ID through the active object registries, and stores the resulting native pointer at offset `0x04`. Later movement, animation, visibility, query, and effect commands operate on this selected object.

This is a mutable execution target, not necessarily immutable ownership: a script can select another object later. Room-level scripts may also remain unbound and operate only on global systems. Consequently, the closest scene-engine analogy is an area-owned script instance with a current receiver, not a script component owned by every scene node.

### Native event dispatch

The recovered native event path uses a fixed 32-slot queue. Its consumer, `func_001335B8`, passes a target object ID, event kind, and payload to `func_001C6068`. That dispatcher selects the active script pool, scans for a runtime record whose ID at offset `0x00` matches the target, checks that the corresponding entry is non-empty, and schedules it through the task system.

The observed event-kind mapping is:

| Native event kind | Script entry |
|------------------:|-------------:|
| 0 | 5 |
| 1 | 6 |
| 2 | 4 |
| 3 | 7 |
| 4 | 3 |
| 5 | Special package/load path |

The semantic names of event kinds 0 through 4 are not established yet. The mapping itself is explicit in `func_001C6068`; do not assign lifecycle names such as `update`, `spawn`, or `destroy` without additional caller evidence.

A separate centralized event path, `func_001C6530`, stores the event-source object ID and message code in shared `ScriptRuntime` context and directly executes entry 10 of the global script record. This is the path used by event and cutscene orchestration rather than object-targeted callback dispatch.

Primary native evidence:

- [`../../asm/nonmatchings/script_exec/func_001C5918.s`](../../asm/nonmatchings/script_exec/func_001C5918.s): runtime-record creation, entry 0 execution, and entry 1 activation
- [`../../asm/C5E08.s`](../../asm/C5E08.s): object-ID lookup, fixed event-entry mapping, scheduling, and direct entry 10 execution
- [`../../asm/DF218.s`](../../asm/DF218.s): command 10 writes the selected-object ID at offset `0x00` and pointer at `0x04`
- [`../../asm/2B230.s`](../../asm/2B230.s): queued native events forwarded to `func_001C6068`

## Files

### `script_command_semantics/`

The authoritative human-reviewed semantics source. Each command has an independently editable JSON record named by its ID, such as `script_command_semantics/157.json`. A record contains:

- Its command ID and review status
- A semantic name and confidence when the meaning is established
- Stack inputs, outputs, confidence, and evidence
- Argument names and types
- Analysis notes and explicit relationships to command variants

Corpus-used commands that cannot be named defensibly have a `reviewed_unresolved` record with a reason instead of a speculative name. A missing record means the command has not been reviewed.

### `script_commands.json`

The generated, complete command database. It contains all 749 command IDs and combines:

- Handler addresses and symbols from the executable and linker map
- Recovered semantics from `script_command_semantics/`
- Stack signatures
- Corpus call counts
- Representative archive, script, entry, and instruction locations

Do not edit this file directly. Regenerate it after changing a record in `script_command_semantics/`.

### `script_command_naming.json`

The shared, read-only vocabulary for command entities, verbs, measurements, and confidence policy. Parallel workers should propose convention changes to the coordinator rather than edit this file during a naming wave.

### Generated evidence

The generator can write machine-derived handler, inferred-stack, call-count, and representative-corpus evidence as independent records under `build/jp/script_command_evidence/`. This build output is an immutable input to a naming wave and is not committed.

### Other files

The remaining files in this directory support the JP executable build and symbol mapping. They are not part of the script command database schema.

## Generator

[`../../tools/script_command_db.py`](../../tools/script_command_db.py) builds and validates the database. Run its commands from the repository root.

Validate the semantic source records:

```sh
python tools/script_command_db.py validate-semantics \
  config/jp/script_command_semantics
```

Regenerate the aggregate database and per-command evidence:

```sh
python tools/script_command_db.py generate \
  --executable SLPS_251.05 \
  --map build/jp/SLPS_251.05.map \
  --input 'Kingdom Hearts (Japan)/Kingdom Hearts (Japan).iso' \
  --semantics config/jp/script_command_semantics \
  --evidence-output build/jp/script_command_evidence \
  --output config/jp/script_commands.json
```

Validate the generated database against the script corpus:

```sh
python tools/script_command_db.py check \
  config/jp/script_commands.json \
  'Kingdom Hearts (Japan)/Kingdom Hearts (Japan).iso'
```

Export collision-checked high-confidence native handler symbols:

```sh
python tools/script_command_db.py export-symbols \
  config/jp/script_commands.json \
  --map build/jp/SLPS_251.05.map \
  --format json \
  --output build/jp/script_command_symbols.json
```

Use `--format pcsx2` to emit a PCSX2 debugger `.sym` file instead. The exporter preserves an existing non-autogenerated handler name, adds a command-ID suffix when semantic aliases would otherwise collide, and rejects names already assigned to another map address.

The generated manifest is the propagation boundary between semantic review and native symbol renaming. Do not maintain a second hand-written command-symbol list.

Generate the committed Splat symbol source from the same database:

```sh
python tools/script_command_db.py export-symbols \
  config/jp/script_commands.json \
  --map build/jp/SLPS_251.05.map \
  --format splat \
  --output config/jp/script_command_symbols.txt
```

`config/kh.jp.yaml` loads this file after `symbol_addrs.txt`, so a clean Splat run emits assembly with the semantic handler names. The file is generated; do not edit it by hand.

Preview exact-token propagation through native sources, generated assembly, declarations, and semantic evidence:

```sh
python tools/script_command_db.py propagate-symbols \
  build/jp/script_command_symbols.json \
  --path src \
  --path include \
  --path asm \
  --path config/jp/script_command_semantics \
  --path config/jp/symbol_addrs.txt \
  --path config/jp/undefined_funcs_auto.txt \
  --path config/jp/undefined_syms_auto.txt
```

Add `--apply` only after reviewing the reported file and replacement counts. Propagation replaces complete symbol tokens, rejects ambiguous old names, and removes superseded autogenerated linker entries. Regenerate from an uncached split and rebuild afterward:

```sh
.venv/bin/python configure.py -c -v jp
ninja
```

## Runtime field recovery

Shared `Script` and `ScriptRuntime` fields are named only when multiple handlers establish their storage role, type, and lifetime. Recover the structure field centrally before renaming locals that consume it. Current recovered fields include the Script bytecode cursor and value stack, event-effect package ID, pause-menu selection, and event-source context.

The executable and ISO are local assets and are not distributed by this repository.

## Related implementation

- [`../../tools/script_bytecode.py`](../../tools/script_bytecode.py): lossless area-script parsing, VM opcode definitions, control-flow and stack analysis, disassembly, and assembly
- [`../../tools/script_command_db.py`](../../tools/script_command_db.py): native handler-table extraction, semantic metadata merging, corpus profiling, generation, and validation
- `script_command_semantics/`: authoritative independently editable semantic records
- `script_command_naming.json`: shared naming vocabulary and review policy
- `script_commands.json`: generated aggregate consumed by analysis and disassembly tools

## Update workflow

1. Generate one immutable per-command evidence snapshot before starting a naming wave.
2. Assign command IDs; each worker edits only its command records and does not regenerate aggregate output.
3. Confirm each handler's behavior, stack effect, argument order, and representative corpus usage.
4. Use `reviewed_unresolved` with a concrete reason when the evidence does not support a defensible name.
5. Run `validate-semantics`; reviewers report findings without editing worker-owned records.
6. Apply review corrections through a single coordinator.
7. Regenerate `script_commands.json` and evidence once after all semantic edits are complete.
8. Run the corpus check and bytecode regression tests.
