param(
    [string]$BuildDir = "out\build\release",
    [string]$StagingDir = "deploy\release",
    [string]$QtBin = "C:\Qt\6.11.1\msvc2022_64\bin"
)

$ErrorActionPreference = "Stop"

$exePath = Join-Path $BuildDir "PrinterCare.exe"
if (-not (Test-Path $exePath)) {
    throw "Release executable not found: $exePath"
}

$windeployqt = Join-Path $QtBin "windeployqt.exe"
if (-not (Test-Path $windeployqt)) {
    throw "windeployqt not found: $windeployqt"
}

New-Item -ItemType Directory -Force -Path $StagingDir | Out-Null
Copy-Item $exePath $StagingDir -Force

& $windeployqt --release --compiler-runtime --dir $StagingDir $exePath
exit $LASTEXITCODE
