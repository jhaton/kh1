#!/usr/bin/env python3
"""Assemble KH1 .khsasm source into a base area archive."""

from __future__ import annotations

import argparse
from pathlib import Path

if __package__:
    from .script_bytecode import FormatError, assemble_archive, parse_khsasm
    from .script_command_db import load_command_database
else:
    from script_bytecode import FormatError, assemble_archive, parse_khsasm
    from script_command_db import load_command_database


def assemble(
    source_path: Path,
    base_path: Path,
    output_path: Path,
    command_database: Path | None = None,
) -> int:
    commands = load_command_database(command_database) if command_database is not None else None
    command_names = (
        {command_id: metadata.display_name for command_id, metadata in commands.items()}
        if commands is not None
        else None
    )
    blocks = parse_khsasm(source_path.read_text(), command_names=command_names)
    rebuilt = assemble_archive(base_path.read_bytes(), blocks)
    output_path.write_bytes(rebuilt)
    print(f"wrote {len(blocks)} script blocks to {output_path}")
    return 0


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("source", type=Path, help="lossless .khsasm source")
    parser.add_argument("base", type=Path, help="original decompressed .ard container")
    parser.add_argument("output", type=Path, help="rebuilt .ard container")
    parser.add_argument("--command-db", type=Path, help="resolve symbolic calls using this command metadata database")
    args = parser.parse_args()

    try:
        return assemble(args.source, args.base, args.output, args.command_db)
    except (FormatError, OSError) as error:
        parser.error(str(error))


if __name__ == "__main__":
    raise SystemExit(main())
