# Documentation

This folder contains the project documentation for `LBR-26-Ground-Software`.

## Contents

- `docs/build-and-run.md`: local build, test, docs and CI-related commands
- `docs/configuration.md`: runtime YAML schema and CLI override behavior
- `docs/architecture.md`: software modules, runtime flow, and transport roles
- `docs/release.md`: tag-driven release automation and published artifacts

Additional contract documentation for connector/protobuf lives in:

- `LoRa/docs/connector.md`
- `LoRa/docs/protobuf-nanopb.md`

## Quick Start

1. Build the project (see `docs/build-and-run.md`)
2. Use `config.demo.yaml` or your own config (see `docs/configuration.md`)
3. Run tests and sanity checks
4. Run the executable and validate logs
