# Repository Guidelines

## Project Overview

Kingdom Hearts De:Compiled reconstructs the PlayStation 2 executables for the original Japanese release (`SLPS_251.05`) and Final Mix (`SLPS_251.98`). This is a matching-focused decompilation: source readability matters, but preserving the target binary's code generation, data layout, symbol addresses, and SHA-1 is the primary constraint. Game assets and executables are local prerequisites and must not be committed.

## Architecture & Data Flow

1. A legally obtained game ELF is placed at the repository root under its serial name.
2. `configure.py` runs Splat with `config/kh.<version>.yaml`, splitting the ELF and generating `asm/`, `assets/`, a linker script, `build.ninja`, and automatic undefined-symbol files.
3. Ninja compiles normal game C with bundled EE GCC 2.96. `src/lib/` uses the older EE GCC 2.9-991111 toolchain. Game compiler assembly passes through `tools/masps2.py` before PS2 assembly.
4. MIPS objects and extracted data are linked, converted to a raw executable, then checked against `config/<version>/checksum.sha1`.

`main` is in `src/xbeginning.c`: it initializes the engine and repeatedly runs the main update function. Runtime code is subsystem-oriented but global-state-heavy. Representative areas include storage (`disk.c`, `memcard.c`), world filenames (`worldfile.c`), Gummi systems (`gumi.c`), and the PPP particle/effect engine (`src/ppp/`). Many functions remain assembly-backed through `INCLUDE_ASM`; C and assembly therefore coexist in one link.

There is no dependency-injection framework, async application framework, or centralized state store. Dependencies are direct function calls, SDK APIs, pointers, fixed arrays, and address-named globals. PS2 threads/semaphores are explicit platform code, not a general async abstraction.

## Key Directories

- `src/`: game translation units. Opaque `x*.c` names and `func_XXXXXXXX` symbols reflect incomplete recovery.
- `src/ppp/`: PPP particle/effect manager plus small program and draw modules.
- `src/lib/`: reconstructed runtime/SDK library code compiled with the older library compiler.
- `include/`: project types, recovered structure layouts, PS2 SDK headers, and assembly-inclusion macros.
- `config/`: Splat layouts and per-version symbol maps, undefined symbols, and expected checksums.
- `tools/cc/`: pinned EE compiler toolchains. Do not replace them with host GCC.
- `tools/iso/`: optional JP ISO extraction and filename-hash utilities.
- `tools/xmaker/`: optional zero-dependency Rust filename-hash brute-forcer.
- `asm/`, `assets/`, `build/`: generated, ignored outputs; they may not exist before configuration.

## Development Commands

Run commands from the repository root.

```sh
python3 -m pip install -U -r requirements.txt

# Original Japanese build (default version is also jp)
./configure.py -v jp
ninja

# Final Mix build
./configure.py -v fm
ninja

# Remove generated asm/assets/build state, then regenerate JP
./configure.py -c -v jp
ninja
```

`-c` deletes `.splache`, `asm/`, `assets/`, and `build/`; use it only when a clean split is intended. Each configure invocation rewrites `build.ninja` for the selected version.

Useful decompilation helpers:

```sh
python3 diff.py -m -o func_00123456     # rebuild and compare one symbol/object
python3 tools/m2ctx.py src/xexample.c    # write ctx.c for mips_to_c
python3 tools/iso/extract.py path/game.iso
python3 tools/iso/hash.py path/in/game   # inspect the game's filename hash
```

ISO extraction writes `kingdom/` and is currently supported only for JP. `tools/iso/extract.py` imports `rich` in addition to the declared dependencies; install it separately if unavailable.

## Code Conventions & Common Patterns

### Matching-sensitive C

- Follow `.clang-format`: 4 spaces, no tabs, 120 columns, left-aligned pointers, no one-line functions/blocks. Include sorting is intentionally disabled. Do not mass-format matching code.
- Use project-width types from `include/common.h`: `s8`/`u8`, `s16`/`u16`, `s32`/`u32`, `s64`/`u64`, `f32`, and `b32` with `TRUE`/`FALSE`.
- Preserve established subsystem naming. Unknown evidence stays explicit: `func_XXXXXXXX`, `D_XXXXXXXX`, `unk_XX`, `argN`, and `UNK_TYPE` are preferable to speculative semantics.
- Recovered structures document offsets (`/* 0x10 */`) and often include unknown padding. Never reorder fields, change widths, or remove padding without proving the layout and all users.
- Expression shape, statement order, casts, control flow, constants, and temporary lifetimes can change EE GCC output. Avoid cleanup refactors unless the assembly comparison remains exact.
- `INCLUDE_ASM("asm/nonmatchings/...", symbol)` is the current implementation for unmatched functions. A tentative C body may live under `#ifdef NON_MATCHING` or in comments. Do not delete the assembly fallback until the C implementation matches.
- Cross-file symbol renames must update declarations, callers, Splat symbol maps, and generated assembly names consistently.

### State, errors, and concurrency

- State commonly lives in fixed-size global arrays, address-named globals, and mutable manager structs; preserve initialization order and sentinel values.
- C functions generally return SDK-style integer/boolean status codes rather than exceptions. Preserve exact return values and side effects.
- Python utilities are standalone `argparse` CLIs and typically report fatal errors with stderr/`sys.exit(1)`. Keep failures explicit; do not silently continue with partial generated data.
- No project-wide lint command is configured. VS Code suggests Black for Python and rustfmt for Rust, but matching-sensitive C should only receive targeted formatting.

## Important Files

- `README.md`: supported releases, required local assets, and basic setup.
- `configure.py`: authoritative build graph, compiler selection, linker, clean behavior, and checksum target.
- `config/kh.jp.yaml`, `config/kh.fm.yaml`: segment layouts and Splat options.
- `config/{jp,fm}/symbol_addrs.txt`: recovered game symbols; `lib_symbol_addrs.txt` covers library symbols.
- `config/{jp,fm}/checksum.sha1`: exact expected executable hashes.
- `src/xbeginning.c`: executable entry point and main loop.
- `include/common.h`: fixed-width aliases, booleans, common data/functions, and unknown-type conventions.
- `include/include_asm.h`: `INCLUDE_ASM` and `INCLUDE_RODATA` behavior.
- `src/ppp/pppMngr.c`, `include/ppp.h`: central PPP state and recovered layouts.
- `tools/masps2.py`: required compiler-assembly hazard fixups.
- `diff.py`, `diff_settings.py`: function/object assembly comparison configuration.
- `decomp.yaml`: decomp.me/permuter metadata.
- `Jenkinsfile`: CI's JP and FM configure/build sequence.

## Runtime/Tooling Preferences

- Use Python 3 and `pip` for build/extraction tooling; dependencies are in `requirements.txt`.
- Use Ninja for builds and system `mips-linux-gnu-*` binutils for assembly/link/strip/objcopy.
- Use the committed compilers under `tools/cc/`; compiler version and flags (`-O2 -G0`, selectively `-g`) are part of matching behavior.
- Rust/Cargo is only needed for `tools/xmaker`; it is not part of the game build.
- Node, Bun, npm, and JavaScript package managers are not used.
- Do not commit local ELFs, ISOs, extracted assets, generated assembly, build products, `.sym` files, or context files; `.gitignore` lists these artifacts.

## Testing & QA

There is no top-level unit-test framework, coverage target, or enforced lint suite. The primary QA contract is an exact build:

1. Configure the affected version.
2. Run `ninja`; a successful full build reaches the SHA-1 check embedded in the generated graph.
3. For source matching work, inspect the affected function with `python3 diff.py -m -o <symbol>` before the full build.
4. Build both `jp` and `fm` when changing shared source, headers, build logic, or tooling used by both versions.

Expected hashes are `9dabbf867a7ec2a030df99ba1ed969f2deef0488` for JP and `e70bda789916142aafb53d85cef2e806b35ad8d8` for Final Mix. If the local game executable is absent, state that verification is blocked rather than substituting host compilation or fabricated assets.
