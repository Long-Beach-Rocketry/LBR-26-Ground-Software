# Build and Run

## Prerequisites

- CMake (>= 3.23)
- Ninja
- C++17 compiler
- PowerShell (for helper scripts on Windows)

Note: `yaml-cpp` is fetched automatically by CMake (`FetchContent`).
GoogleTest is also fetched automatically when `BUILD_TESTING=ON` (default).

## Build (PowerShell)

Recommended command from repository root:

```powershell
.\dev.ps1 build
```

Build only (skip tests):

```powershell
.\dev.ps1 build-only
```

Direct preset configure/build on Windows UCRT64:

```powershell
cmake --preset windows-ucrt-ninja
cmake --build --preset windows-ucrt-ninja --target lbr_tests
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
.\build\LoRa\lbr_ground.exe --help
.\build\LoRa\lbr_ground.exe -c .\LoRa\config.demo.yaml
.\build\LoRa\lbr_ground.exe -c .\LoRa\config.demo.yaml -v
```

HIL runner:

```powershell
.\build\LoRa\lbr_hil_runner.exe --module sx1262 --timeout-ms 5000 --min-bytes 1
```

## Expected Behavior

- `--help` prints usage and exits with success.
- `-c <file>` loads YAML configuration and validates it.
- `-v` forces verbose mode even if `pipeline.verbose` is false in YAML.

## CI Checks

Current CI workflow includes:

- build and test
- coverage
- feature-options (ZeroMQ + protobuf/nanopb)
- Windows smoke
- clang sanity (`clang-format-check`, `clang-tidy`)
- docs build
