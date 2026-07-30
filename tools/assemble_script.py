#!/usr/bin/env python3
"""Assemble KH1 .khsasm source into a base area archive."""

from __future__ import annotations

import argparse
from pathlib import Path

from script_bytecode import FormatError, assemble_archive, parse_khsasm


def assemble(source_path: Path, base_path: Path, output_path: Path) -> int:
    blocks = parse_khsasm(source_path.read_text())
    rebuilt = assemble_archive(base_path.read_bytes(), blocks)
    output_path.write_bytes(rebuilt)
    print(f"wrote {len(blocks)} script blocks to {output_path}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="lossless .khsasm source")
    parser.add_argument("base", type=Path, help="original decompressed .ard container")
    parser.add_argument("output", type=Path, help="rebuilt .ard container")
    args = parser.parse_args()

    try:
        return assemble(args.source, args.base, args.output)
    except (FormatError, OSError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
