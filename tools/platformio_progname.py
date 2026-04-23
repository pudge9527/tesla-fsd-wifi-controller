Import("env")
import os
from pathlib import Path
import subprocess

if env.get("BOARD_MCU") in {"esp32", "esp32c3", "esp32c5", "esp32c6", "esp32s3"}:
    os.environ.setdefault("IDF_COMPONENT_MANAGER", "0")

env.Replace(PROGNAME=env.GetProjectOption("custom_progname", "fsd-controller"))

app_version = os.getenv("APP_VERSION", "").strip()
if app_version:
    escaped_app_version = app_version.replace("\\", "\\\\").replace('"', '\\"')
    env.AppendUnique(
        CPPDEFINES=[("APP_VERSION", f'\\"{escaped_app_version}\\"')]
    )


def ensure_insights_https_cert_source():
    if env.get("BOARD_MCU") not in {"esp32c3", "esp32c6"}:
        return

    project_dir = Path(env.subst("$PROJECT_DIR"))
    build_dir = Path(env.subst("$BUILD_DIR"))
    build_dir.mkdir(parents=True, exist_ok=True)

    framework_espidf = env.PioPlatform().get_package_dir("framework-espidf")
    tool_cmake = env.PioPlatform().get_package_dir("tool-cmake")
    if not framework_espidf or not tool_cmake:
        return

    cmake_bin = Path(tool_cmake) / "bin" / "cmake"
    script = Path(framework_espidf) / "tools" / "cmake" / "scripts" / "data_file_embed_asm.cmake"
    mappings = [
        (
            project_dir / "managed_components" / "espressif__esp_insights" / "server_certs" / "https_server.crt",
            build_dir / "https_server.crt.S",
        ),
        (
            project_dir / "managed_components" / "espressif__esp_rainmaker" / "server_certs" / "rmaker_mqtt_server.crt",
            build_dir / "rmaker_mqtt_server.crt.S",
        ),
        (
            project_dir / "managed_components" / "espressif__esp_rainmaker" / "server_certs" / "rmaker_claim_service_server.crt",
            build_dir / "rmaker_claim_service_server.crt.S",
        ),
        (
            project_dir / "managed_components" / "espressif__esp_rainmaker" / "server_certs" / "rmaker_ota_server.crt",
            build_dir / "rmaker_ota_server.crt.S",
        ),
    ]

    for cert_path, asm_path in mappings:
        if asm_path.exists():
            continue
        actual_cert = cert_path if cert_path.exists() else (project_dir / "tools" / "placeholder_server.crt")
        subprocess.run(
            [
                str(cmake_bin),
                f"-D DATA_FILE={actual_cert}",
                f"-D SOURCE_FILE={asm_path}",
                "-D FILE_TYPE=TEXT",
                "-P",
                str(script),
            ],
            check=True,
        )


ensure_insights_https_cert_source()


def prepare_insights_https_cert(*_args, **_kwargs):
    cert_path = Path(env.subst("$PROJECT_DIR")) / "managed_components" / "espressif__esp_insights" / "server_certs" / "https_server.crt"
    if not cert_path.exists():
        return

    build_dir = Path(env.subst("$BUILD_DIR"))
    build_dir.mkdir(parents=True, exist_ok=True)
    asm_path = build_dir / "https_server.crt.S"
    if asm_path.exists():
        return

    framework_espidf = env.PioPlatform().get_package_dir("framework-espidf")
    tool_cmake = env.PioPlatform().get_package_dir("tool-cmake")
    if not framework_espidf or not tool_cmake:
        return

    cmake_bin = Path(tool_cmake) / "bin" / "cmake"
    script = Path(framework_espidf) / "tools" / "cmake" / "scripts" / "data_file_embed_asm.cmake"
    subprocess.run(
        [
            str(cmake_bin),
            f"-D DATA_FILE={cert_path}",
            f"-D SOURCE_FILE={asm_path}",
            "-D FILE_TYPE=TEXT",
            "-P",
            str(script),
        ],
        check=True,
    )


env.AddPreAction("buildprog", prepare_insights_https_cert)
