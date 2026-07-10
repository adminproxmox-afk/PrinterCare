param(
    [ValidateSet("app", "smoke")]
    [string]$Mode = "app",
    [string]$QtBin = "C:\Qt\6.11.1\msvc2022_64\bin",
    [string]$QtPlugins = "C:\Qt\6.11.1\msvc2022_64\plugins",
    [string]$BuildDir = "out\build\debug"
)

$ErrorActionPreference = "Stop"

$env:PATH = "$QtBin;$env:PATH"
$env:QT_PLUGIN_PATH = $QtPlugins
$env:QT_QPA_PLATFORM_PLUGIN_PATH = Join-Path $QtPlugins "platforms"

$exeName = if ($Mode -eq "smoke") { "PrinterCareSmoke.exe" } else { "PrinterCare.exe" }
$exePath = Join-Path $BuildDir $exeName

if (-not (Test-Path $exePath)) {
    throw "Executable not found: $exePath"
}

& $exePath
exit $LASTEXITCODE
