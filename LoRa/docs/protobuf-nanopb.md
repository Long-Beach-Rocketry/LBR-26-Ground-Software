# Protobuf / Nanopb Integration

This repository keeps the connector wire schema in:

- `docs/connector-message.proto`

For constrained targets, nanopb can generate bounded C structs from the same schema.

## CMake Targets

Configure with optional code generation enabled:

```powershell
cmake -S . -B build -DLBR_ENABLE_PROTOBUF_CODEGEN=ON
```

Generate C++ protobuf sources (ground side):

```powershell
cmake --build build --target connector_proto_codegen
```

Generate nanopb sources (embedded side):

```powershell
cmake --build build --target connector_nanopb_codegen
```

Generated files are written to:

- `build/generated/proto`

## Notes

- `connector-message.nanopb.options` defines bounded field sizes for nanopb.
- Keep schema changes backward-compatible when possible (increment semantic contract version in metadata when needed).
- ZeroMQ/UDP/TCP transports carry serialized bytes; transport and schema evolution should remain decoupled.
