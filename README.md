# Kingdom Hearts De:Compiled

An in-progress decompilation of the PlayStation 2 releases of **Kingdom Hearts**.

The project reconstructs the original game executable from C and assembly. Binary matching is the primary correctness target: a successful full build must reproduce the expected SHA-1 exactly. Many functions are still assembly-backed while their C implementations are recovered.

> [!IMPORTANT]
> This repository does not contain game executables, disc images, or extracted assets. You must provide files from a legally obtained copy of the game.

## Supported Versions

| Release | Configure target | Required ELF | Build output | Expected SHA-1 |
|---|---|---|---|---|
| Original Japanese | `jp` | `SLPS_251.05` | `build/jp/SLPS_251.05` | `9dabbf867a7ec2a030df99ba1ed969f2deef0488` |
| Final Mix | `fm` | `SLPS_251.98` | `build/fm/SLPS_251.98` | `e70bda789916142aafb53d85cef2e806b35ad8d8` |

## Requirements

- Python 3 and `pip`
- [Ninja](https://ninja-build.org/)
- GNU MIPS cross-binutils available as `mips-linux-gnu-*`
- The target game's main ELF, named exactly as shown above

The matching EE GCC toolchains are included under `tools/cc/`; do not replace them with a host compiler.

Install the Python dependencies:

```sh
python3 -m pip install -U -r requirements.txt
```

## Building

Run all commands from the repository root.

### Original Japanese

```sh
# Place SLPS_251.05 in the repository root first.
./configure.py -v jp
ninja
```

`jp` is the default, so `./configure.py` without `-v` selects the same version.

### Final Mix

```sh
# Place SLPS_251.98 in the repository root first.
./configure.py -v fm
ninja
```

`configure.py` runs Splat to generate assembly, assets, the linker script, and `build.ninja`. Ninja then compiles and links the selected version. The final build step checks the output against `config/<version>/checksum.sha1`; a checksum mismatch fails the build.

### Clean regeneration

```sh
./configure.py -c -v jp
ninja
```

> [!WARNING]
> `-c` removes `.splache`, `asm/`, `assets/`, and `build/` before regenerating them. Use the version you intend to build after cleaning.

Each configure invocation rewrites `build.ninja` for one version. Re-run `configure.py` when switching between `jp` and `fm`.

## Repository Layout

| Path | Purpose |
|---|---|
| `src/` | Decompiled game C and reconstructed runtime/library sources |
| `src/ppp/` | PPP particle and effect engine modules |
| `include/` | Project types, recovered structures, PS2 SDK headers, and assembly macros |
| `config/kh.*.yaml` | Per-version Splat segment layouts |
| `config/jp/`, `config/fm/` | Symbol maps, undefined symbols, and expected checksums |
| `tools/cc/` | Bundled matching EE GCC toolchains |
| `tools/iso/` | Optional JP asset extraction and filename-hash tools |
| `asm/`, `assets/`, `build/` | Generated and ignored build artifacts |

## Decompilation Notes

- Unmatched functions use `INCLUDE_ASM("asm/nonmatchings/...", symbol)` until their C implementation reproduces the target assembly.
- Address-based names such as `func_00123456`, `D_00123456`, and `unk_10` indicate unresolved semantics. Prefer evidence-backed names over guesses.
- Structure offsets, padding, integer widths, expression order, and control flow can all affect EE GCC output.
- `.clang-format` documents the C style, but avoid broad formatting changes in matching-sensitive code.
- Run a full `ninja` build after source, header, symbol-map, compiler, or build-system changes.

See [`AGENTS.md`](AGENTS.md) for detailed repository conventions and guidance for automated contributors.

## Optional JP Asset Extraction

Additional assets are not required for the executable build. For the original Japanese release, they can be extracted from a legal ISO:

```sh
# The extractor imports rich in addition to the standard project requirements.
python3 -m pip install rich
python3 tools/iso/extract.py /path/to/KingdomHearts.iso
```

Files are written to `kingdom/`, which is ignored by Git. Final Mix asset extraction is not currently supported.

## Continuous Integration

`Jenkinsfile` configures and builds both supported versions. CI requires legal local copies of both game executables and validates each reconstructed output through the same SHA-1 build target.
