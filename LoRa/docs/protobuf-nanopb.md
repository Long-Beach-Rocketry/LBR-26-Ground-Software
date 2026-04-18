# Protobuf / Nanopb Integration

This repository keeps the connector wire schema in:

- `docs/connector-message.proto`

For constrained targets, nanopb can generate bounded C structs from the same schema.

## CMake Targets

Configure with optional code generation enabled:

```powershell
.\dev.ps1 proto
```

Generate C++ protobuf sources (ground side):

```powershell
.\dev.ps1 proto
```

Generate nanopb sources (embedded side):

```powershell
.\dev.ps1 nanopb
```

Generated files are written to:

- `build/generated/proto`

## Notes

- `connector-message.nanopb.options` defines bounded field sizes for nanopb.
- Keep schema changes backward-compatible when possible (increment semantic contract version in metadata when needed).
- ZeroMQ/UDP/TCP transports carry serialized bytes; transport and schema evolution should remain decoupled.
