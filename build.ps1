param(
    [string]$Preset = "windows-ucrt-ninja",
    [string]$BuildTarget = "lbr_tests",
    [switch]$SkipTests
)

$scriptPath = Join-Path $PSScriptRoot "tools\build.ps1"
& $scriptPath -Preset $Preset -BuildTarget $BuildTarget -SkipTests:$SkipTests
exit $LASTEXITCODE
