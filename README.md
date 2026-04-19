# LBR-26 Ground Software

Ground-side software stack for Long Beach Rocketry, centered on the `LoRa/` module and built with CMake.

## Overview

The project currently provides:

- LoRa runtime abstraction with module selection (`sx1262`, `sx127`)
- runtime mode selection (`virtual`, `hardware`)
- connector transports:
  - local TCP
  - local UDP
  - optional local ZeroMQ
  - NDJSON file replay
- protobuf and nanopb code generation targets for connector contracts
- unit and integration tests (GoogleTest + CTest)

Primary implementation lives under [LoRa](LoRa).

## Quick Start

From repository root:

```powershell
.\dev.ps1 build
```

Build-only:

```powershell
.\dev.ps1 build-only
```

Run tests:

```powershell
.\dev.ps1 test
```

Run app:

```powershell
.\build\LoRa\lbr_ground.exe --help
.\build\LoRa\lbr_ground.exe -c .\LoRa\config.demo.yaml -v
```

## CI/CD

Main CI workflow runs:

- build and tests
- coverage generation
- optional feature validation (ZeroMQ + protobuf/nanopb)
- Windows UCRT smoke build/test
- clang checks (`clang-format-check`, `clang-tidy`)
- docs build (Doxygen HTML)

Workflow file: [.github/workflows/ci.yml](.github/workflows/ci.yml).

## Releases

Release automation is handled by [.github/workflows/release.yml](.github/workflows/release.yml).

On tag `vX.Y.Z`, it builds and publishes:

- Linux binaries archive
- Windows UCRT64 binaries archive
- Doxygen HTML archive

Full release procedure: [docs/release.md](docs/release.md).

## Documentation Index

- [docs/build-and-run.md](docs/build-and-run.md)
- [docs/configuration.md](docs/configuration.md)
- [docs/architecture.md](docs/architecture.md)
- [docs/release.md](docs/release.md)
- [LoRa/docs/connector.md](LoRa/docs/connector.md)
- [LoRa/docs/protobuf-nanopb.md](LoRa/docs/protobuf-nanopb.md)
