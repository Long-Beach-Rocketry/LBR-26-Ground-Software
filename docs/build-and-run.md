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
.\tools\build.ps1
```

Build only (skip tests):

```powershell
.\tools\build.ps1 -SkipTests
```

Equivalent manual commands:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"
cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM=C:\msys64\ucrt64\bin\ninja.exe -DCMAKE_CXX_COMPILER=C:\msys64\ucrt64\bin\g++.exe -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

## Test

```powershell
ctest --test-dir build --output-on-failure
```

This runs:
- unit test binary (`lbr_tests`, based on GoogleTest)
- integration checks on `lbr_ground` (`--help`, demo config, missing config failure path)

## Clang Sanity Check

When `clang-format` and `clang-tidy` are installed in `PATH`, CMake exposes sanity targets:

```powershell
cmake --build build --target clang-format-check
cmake --build build --target clang-tidy
cmake --build build --target sanity-check
```

If either tool is missing from `PATH`, the corresponding target fails with an explicit message.

## Doxygen Documentation

Generate API documentation:

```powershell
cmake --build build --target docs
```

Generated HTML entry point:

- `build/docs/doxygen/html/index.html`

Generate PDF documentation:

```powershell
cmake --build build --target docs-pdf
```

Generated PDF:

- `build/docs/doxygen/latex/refman.pdf`

If `doxygen` is missing from `PATH`, the target fails with an explicit message.

Optional compile-time tidy enforcement:

```powershell
cmake -S . -B build -DLBR_ENABLE_CLANG_TIDY=ON
cmake --build build -j
```

Automatic formatting target:

```powershell
cmake --build build --target clang-format
```

## Coverage

Build with coverage instrumentation:

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"
cmake -S . -B build-coverage -G Ninja -DCMAKE_MAKE_PROGRAM=C:\msys64\ucrt64\bin\ninja.exe -DCMAKE_CXX_COMPILER=C:\msys64\ucrt64\bin\g++.exe -DCMAKE_BUILD_TYPE=Debug -DLBR_ENABLE_COVERAGE=ON
cmake --build build-coverage -j
cmake --build build-coverage --target coverage
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
