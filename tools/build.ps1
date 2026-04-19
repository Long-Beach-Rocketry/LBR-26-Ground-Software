param(
    [string]$Preset = "windows-ucrt-ninja",
    [string]$BuildTarget = "lbr_tests",
    [switch]$SkipTests
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][scriptblock]$Action
    )

    Write-Host "==> $Name" -ForegroundColor Cyan
    & $Action
}

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $repoRoot

# Keep MSYS2 runtime DLLs ahead of other toolchains in PATH.
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"

Invoke-Step -Name "Configure preset '$Preset'" -Action {
    cmake --preset $Preset
}

Invoke-Step -Name "Build target '$BuildTarget'" -Action {
    cmake --build --preset $Preset --target $BuildTarget
}

if (-not $SkipTests) {
    Invoke-Step -Name "Run tests preset '$Preset'" -Action {
        ctest --preset $Preset
    }
}

Write-Host "Build workflow completed." -ForegroundColor Green
