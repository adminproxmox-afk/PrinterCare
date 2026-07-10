# PrinterCare

## Build

Use the Qt 6.11.1 MSVC 2022 toolchain from Visual Studio Developer Command Prompt or PowerShell:

```powershell
cmake -S . -B out/build/debug -G Ninja -DQt6_DIR=C:/Qt/6.11.1/msvc2022_64/lib/cmake/Qt6
cmake --build out/build/debug
```

## Run the app

Launch the desktop app:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\run-dev.ps1 -Mode app
```

## Run the integration smoke test

This checks:

- config save/load round-trip
- service history save/load round-trip
- reminder evaluation
- Windows startup registration
- tray initialization and notification dispatch

Run it from an interactive Windows desktop session:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\run-dev.ps1 -Mode smoke
```

If you build with CTest enabled, you can also run:

```powershell
ctest --test-dir out/build/debug -C Debug
```

## Release build and deployment

Build the release configuration:

```powershell
cmake -S . -B out/build/release -G Ninja -DQt6_DIR=C:/Qt/6.11.1/msvc2022_64/lib/cmake/Qt6 -DCMAKE_BUILD_TYPE=Release
cmake --build out/build/release
```

Create a portable folder with Qt runtime files:

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\deploy-release.ps1 -BuildDir out\build\release -StagingDir deploy\release
```

The files needed next to `PrinterCare.exe` are the ones copied by `windeployqt` into `deploy\release`.

## Installer

Open `installer\PrinterCare.iss` in Inno Setup and build it after running the deploy script above.
The resulting installer output is `PrinterCareSetup.exe`.
