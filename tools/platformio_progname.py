Import("env")
import os

env.Replace(PROGNAME="fsd-controller")

app_version = os.getenv("APP_VERSION", "").strip()
if app_version:
    escaped_app_version = app_version.replace("\\", "\\\\").replace('"', '\\"')
    env.AppendUnique(
        CPPDEFINES=[("APP_VERSION", f'\\"{escaped_app_version}\\"')]
    )
