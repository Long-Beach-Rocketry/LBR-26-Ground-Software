# Build and Run

## Prerequisites

- CMake
- MSYS2 UCRT64 toolchain
- `g++` (`C:\msys64\ucrt64\bin\g++.exe`)
- `ninja` (`C:\msys64\ucrt64\bin\ninja.exe`)

Note: `yaml-cpp` is fetched automatically by CMake (`FetchContent`).
GoogleTest is also fetched automatically when `BUILD_TESTING=ON` (default).

## Build (PowerShell)

Recommended single command from repository root:

```powershell
.\dev.ps1 build
```

Build only (skip tests):

```powershell
.\dev.ps1 build-only
```

## Test

```powershell
.\dev.ps1 test
```

This runs:
- unit test binary (`lbr_tests`, based on GoogleTest)
- integration checks on `lbr_ground` (`--help`, demo config, missing config failure path)

## Clang Sanity Check

When `clang-format` and `clang-tidy` are installed in `PATH`, CMake exposes sanity targets:

```powershell
.\dev.ps1 sanity
```

If either tool is missing from `PATH`, the corresponding target fails with an explicit message.

## Doxygen Documentation

Generate API documentation:

```powershell
.\dev.ps1 docs
```

Generated HTML entry point:

- `build/docs/doxygen/html/index.html`

Generate PDF documentation:

```powershell
.\dev.ps1 docs-pdf
```

Generated PDF:

- `build/docs/doxygen/latex/refman.pdf`

If `doxygen` is missing from `PATH`, the target fails with an explicit message.

Optional compile-time tidy enforcement:

```powershell
.\dev.ps1 tidy
```

Automatic formatting target:

```powershell
.\dev.ps1 format
```

## Coverage

Build with coverage instrumentation:

```powershell
.\dev.ps1 coverage
```

Coverage artifacts are generated in:
- `build-coverage/coverage/*.gcov`

## Run

```powershell
.\build\lbr_ground.exe --help
.\build\lbr_ground.exe -c .\config.demo.yaml
.\build\lbr_ground.exe -c .\config.demo.yaml -v
```

## Expected Behavior

- `--help` prints usage and exits with success.
- `-c <file>` loads YAML configuration and validates it.
- `-v` forces verbose mode even if `pipeline.verbose` is false in YAML.
