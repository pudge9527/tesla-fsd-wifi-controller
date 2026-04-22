#!/usr/bin/env python3
from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from pathlib import Path


def resolve_input_file(build_dir: Path, configured_name: str, fallback_name: str | None = None) -> Path:
    configured_path = build_dir / configured_name
    if configured_path.exists():
        return configured_path
    if fallback_name:
        fallback_path = build_dir / fallback_name
        if fallback_path.exists():
            return fallback_path
    raise FileNotFoundError(f"Unable to locate build artifact: {configured_name}")


def resolve_esptool_command() -> list[str]:
    python_candidates = [
        Path.home() / ".platformio" / "penv" / "bin" / "python",
        Path.home() / ".platformio" / "penv" / "bin" / "python3",
    ]

    def wrap_script(script_path: str) -> list[str]:
        for python_path in python_candidates:
            if python_path.is_file():
                return [str(python_path), script_path]
        return [sys.executable, script_path]

    path_candidates = [
        Path.home() / ".platformio" / "penv" / "bin" / "esptool",
        Path.home() / ".platformio" / "penv" / "bin" / "esptool.py",
        Path.home() / ".platformio" / "packages" / "tool-esptoolpy" / "esptool",
        Path.home() / ".platformio" / "packages" / "tool-esptoolpy" / "esptool.py",
    ]
    for candidate in path_candidates:
        if not candidate.is_file():
            continue
        return wrap_script(str(candidate)) if candidate.suffix == ".py" else [str(candidate)]

    which_candidates = [
        shutil.which("esptool"),
        shutil.which("esptool.py"),
    ]
    for candidate in which_candidates:
        if not candidate:
            continue
        return wrap_script(candidate) if candidate.endswith(".py") else [candidate]

    raise FileNotFoundError("Unable to locate esptool executable")


def main() -> int:
    parser = argparse.ArgumentParser(description="Create OTA/full firmware packages from a PlatformIO build directory.")
    parser.add_argument("--build-dir", required=True, help="PlatformIO build directory, e.g. .pio/build/esp32c5")
    parser.add_argument("--app-bin", required=True, help="Application binary filename inside build dir")
    parser.add_argument("--ota-name", required=True, help="Output OTA package filename")
    parser.add_argument("--full-name", required=True, help="Output merged full package filename")
    parser.add_argument("--output-dir", required=True, help="Directory to write packaged firmware files")
    parser.add_argument("--flash-size", help="Override flash size for merged full image")
    args = parser.parse_args()

    build_dir = Path(args.build_dir).resolve()
    output_dir = Path(args.output_dir).resolve()
    output_dir.mkdir(parents=True, exist_ok=True)

    flasher_args = json.loads((build_dir / "flasher_args.json").read_text(encoding="utf-8"))
    flash_settings = flasher_args["flash_settings"]
    flash_files = flasher_args["flash_files"]
    chip = flasher_args.get("extra_esptool_args", {}).get("chip")

    bootloader = resolve_input_file(build_dir, Path(flasher_args["bootloader"]["file"]).name, "bootloader.bin")
    partitions = resolve_input_file(build_dir, Path(flasher_args["partition-table"]["file"]).name, "partitions.bin")
    app = resolve_input_file(build_dir, args.app_bin, Path(flasher_args["app"]["file"]).name)

    ota_output = output_dir / args.ota_name
    full_output = output_dir / args.full_name
    shutil.copy2(app, ota_output)

    flash_size = args.flash_size or flash_settings["flash_size"]
    esptool_cmd = resolve_esptool_command()
    merge_cmd = [
        *esptool_cmd,
        "--chip",
        chip or "auto",
        "merge-bin",
        "--output",
        str(full_output),
        "--flash-mode",
        flash_settings["flash_mode"],
        "--flash-freq",
        flash_settings["flash_freq"],
        "--flash-size",
        flash_size,
        flasher_args["bootloader"]["offset"],
        str(bootloader),
        flasher_args["partition-table"]["offset"],
        str(partitions),
        flasher_args["app"]["offset"],
        str(app),
    ]
    subprocess.run(merge_cmd, check=True)

    print(ota_output)
    print(full_output)
    return 0


if __name__ == "__main__":
    sys.exit(main())
