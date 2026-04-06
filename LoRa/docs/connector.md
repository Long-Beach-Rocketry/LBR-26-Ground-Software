# Connector Contract

This document defines the recommended transport boundary between the LoRa/pipeline side and the consumer side handled by the other team.

The code implementation lives under:

- [include/connector/message.h](../include/connector/message.h)
- [include/connector/ndjson_file.h](../include/connector/ndjson_file.h)
- [include/connector/local_tcp_transport.h](../include/connector/local_tcp_transport.h)

## Recommendation

Use a local socket transport for live communication.

Reasoning:

- It keeps data exchange explicit and low-latency.
- It works well for streaming telemetry frames.
- It avoids file polling and partial-write ambiguity.
- It stays usable on the same machine without exposing a network service.

Recommended implementation choice:

- On Windows: localhost TCP or a named pipe.
- On Unix-like systems: localhost TCP or a Unix domain socket.

If the consumer side already has a preference, keep the transport behind a small adapter so the pipeline contract does not change.

## File Transport Fallback

Use a file-based transport only for replay, offline debugging, or simple handoff between tools.

File transport is acceptable when:

- telemetry is not real-time
- the consumer can tolerate polling
- the goal is traceability and reproducibility

File transport is not the preferred live path because it adds polling, locking, and ordering concerns.

When file transport is used, write one JSON object per line using NDJSON so the same message can be replayed or tailed without parsing a wrapper file.

## Contract Scope

The connector is responsible for:

- receiving telemetry frames or frame-like payloads
- forwarding them to the consumer side without parsing business logic
- preserving ordering within a stream session
- reporting transport errors clearly

The connector is not responsible for:

- LoRa demodulation
- SDR acquisition
- GUI rendering
- application-level interpretation of telemetry

## Suggested Message Shape

Use the JSON envelope described in [connector-message.schema.json](connector-message.schema.json).

Required fields:

- `schema_version`: integer, currently `1`
- `message_type`: string, currently `telemetry_frame`
- `sequence`: monotonically increasing integer starting at `0`
- `timestamp_ms`: Unix epoch milliseconds
- `source`: string, one of `sx1262`, `sx127`, or `simulated`
- `payload_b64`: base64-encoded raw telemetry bytes

Optional fields:

- `checksum_hex`: integrity marker in hexadecimal if the producer wants it
- `metadata`: extra implementation-specific fields that do not change the contract

Example message:

```json
{
	"schema_version": 1,
	"message_type": "telemetry_frame",
	"sequence": 42,
	"timestamp_ms": 1712345678901,
	"source": "sx1262",
	"payload_b64": "AQIDBA==",
	"checksum_hex": "4A1F9C2B"
}
```

That structure is enough for the other team to build a consumer without coupling it to the radio implementation.

## Lifecycle

1. Consumer starts and opens the local transport.
2. Producer initializes the LoRa side and begins publishing frames.
3. Consumer receives ordered payloads and handles them independently.
4. On shutdown, the connector closes cleanly and reports the final state.

## Integration Rule

Do not let the connector leak into `SDRPipeline` or the LoRa radio abstraction.

The pipeline should continue to depend on `ILoRaModule` only. The connector should remain a separate adapter owned by the consumer-facing layer.

## Notes For The Other Team

If they need to change transport later, they should replace only the connector adapter, not the pipeline or radio backends.

The formal schema is in [connector-message.schema.json](connector-message.schema.json).