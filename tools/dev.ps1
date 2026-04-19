param(
    [ValidateSet("help", "build", "build-only", "test", "sanity", "format", "tidy", "docs", "docs-pdf", "coverage", "proto", "nanopb", "hil")]
    [string]$Command = "help"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..")
Set-Location $repoRoot

# Keep MSYS2 runtime DLLs ahead of other toolchains.
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"

function Invoke-Step {
    param(
        [Parameter(Mandatory = $true)][string]$Name,
        [Parameter(Mandatory = $true)][scriptblock]$Action
    )

    Write-Host "==> $Name" -ForegroundColor Cyan
    & $Action
}

function Show-Help {
    Write-Host "Usage: .\dev.ps1 <keyword>" -ForegroundColor Yellow
    Write-Host ""
    Write-Host "Keywords:" -ForegroundColor Yellow
    Write-Host "  help        Show this help"
    Write-Host "  build       Configure + build lbr_tests + run tests"
    Write-Host "  build-only  Configure + build lbr_tests (no tests)"
    Write-Host "  test        Run test preset"
    Write-Host "  sanity      Run clang-format-check + clang-tidy"
    Write-Host "  format      Run clang-format"
    Write-Host "  tidy        Run clang-tidy"
    Write-Host "  docs        Build Doxygen HTML"
    Write-Host "  docs-pdf    Build Doxygen PDF"
    Write-Host "  coverage    Configure and build coverage target"
    Write-Host "  proto       Generate C++ protobuf sources"
    Write-Host "  nanopb      Generate nanopb sources"
    Write-Host "  hil         Build HIL runner and print real bench command"
}

switch ($Command) {
    "help" {
        Show-Help
    }

    "build" {
        & (Join-Path $repoRoot "build.ps1")
        exit $LASTEXITCODE
    }

    "build-only" {
        & (Join-Path $repoRoot "build.ps1") -SkipTests
        exit $LASTEXITCODE
    }

    "test" {
        Invoke-Step -Name "Run tests preset 'windows-ucrt-ninja'" -Action {
            ctest --preset windows-ucrt-ninja
        }
    }

    "sanity" {
        Invoke-Step -Name "Run sanity-check target" -Action {
            cmake --build --preset windows-ucrt-ninja --target sanity-check
        }
    }

    "format" {
        Invoke-Step -Name "Run clang-format target" -Action {
            cmake --build --preset windows-ucrt-ninja --target clang-format
        }
    }

    "tidy" {
        Invoke-Step -Name "Run clang-tidy target" -Action {
            cmake --build --preset windows-ucrt-ninja --target clang-tidy
        }
    }

    "docs" {
        Invoke-Step -Name "Build docs target" -Action {
            cmake --build --preset windows-ucrt-ninja --target docs
        }
    }

    "docs-pdf" {
        Invoke-Step -Name "Build docs-pdf target" -Action {
            cmake --build --preset windows-ucrt-ninja --target docs-pdf
        }
    }

    "coverage" {
        Invoke-Step -Name "Configure coverage build" -Action {
            cmake -S . -B build-coverage -G Ninja -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_BUILD_TYPE=Debug -DLBR_ENABLE_COVERAGE=ON
        }

        Invoke-Step -Name "Build coverage artifacts" -Action {
            cmake --build build-coverage -j
        }

        Invoke-Step -Name "Run coverage target" -Action {
            cmake --build build-coverage --target coverage
        }
    }

    "proto" {
        Invoke-Step -Name "Configure build with protobuf codegen" -Action {
            cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_BUILD_TYPE=Release -DLBR_ENABLE_PROTOBUF_CODEGEN=ON
        }

        Invoke-Step -Name "Generate connector protobuf C++ sources" -Action {
            cmake --build build --target connector_proto_codegen
        }
    }

    "nanopb" {
        Invoke-Step -Name "Configure build with protobuf codegen" -Action {
            cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM=C:/msys64/ucrt64/bin/ninja.exe -DCMAKE_CXX_COMPILER=C:/msys64/ucrt64/bin/g++.exe -DCMAKE_BUILD_TYPE=Release -DLBR_ENABLE_PROTOBUF_CODEGEN=ON
        }

        Invoke-Step -Name "Generate connector nanopb sources" -Action {
            cmake --build build --target connector_nanopb_codegen
        }
    }

    "hil" {
        Invoke-Step -Name "Configure preset 'windows-ucrt-ninja'" -Action {
            cmake --preset windows-ucrt-ninja
        }

        Invoke-Step -Name "Build HIL runner" -Action {
            cmake --build --preset windows-ucrt-ninja --target lbr_hil_runner
        }

        Write-Host "Run on bench:" -ForegroundColor Yellow
        Write-Host "  .\\build\\lbr_hil_runner.exe --module sx1262 --timeout-ms 5000 --min-bytes 1"
        Write-Host "  .\\build\\lbr_hil_runner.exe --module sx127 --timeout-ms 5000 --min-bytes 1"
    }

    default {
        throw "Unknown keyword: $Command"
    }
}

Write-Host "Done." -ForegroundColor Green
