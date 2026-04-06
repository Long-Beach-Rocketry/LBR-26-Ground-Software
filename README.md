# LBR-26-Ground-Software

Ground software scaffold for the Long Beach Rocketry SDR station.

Core implementation now lives under `LoRa/`.

## Documentation

- Overview: `docs/README.md`
- Build and run: `docs/build-and-run.md`
- Configuration format: `docs/configuration.md`
- Architecture: `docs/architecture.md`
- Tests and coverage: `docs/build-and-run.md`
- LoRa module details: `LoRa/README.md`

## CI/CD

GitHub Actions workflow:
- `.github/workflows/ci.yml`

It runs:
- build + tests (`ctest`) on pushes and pull requests
- coverage generation (`gcovr`) with artifacts:
  - `coverage.xml`
  - `coverage.html`

## Quick Start

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;C:\msys64\usr\bin;$env:PATH"
cmake -S . -B build -G Ninja -DCMAKE_MAKE_PROGRAM=C:\msys64\ucrt64\bin\ninja.exe -DCMAKE_CXX_COMPILER=C:\msys64\ucrt64\bin\g++.exe -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
.\build\lbr_ground.exe -c .\config.demo.yaml -v
```

For the current layout, prefer:

```powershell
.\build\LoRa\lbr_ground.exe -c .\LoRa\config.demo.yaml -v
```
