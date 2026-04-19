# Connector Contract

This document defines the recommended transport boundary between the LoRa/pipeline side and the consumer side handled by the other team.

The code implementation lives under:

- [include/connector/message.h](../include/connector/message.h)
- [include/connector/ndjson_file.h](../include/connector/ndjson_file.h)
- [include/connector/local_tcp_transport.h](../include/connector/local_tcp_transport.h)
- [include/connector/local_udp_transport.h](../include/connector/local_udp_transport.h)
- [include/connector/local_zmq_transport.h](../include/connector/local_zmq_transport.h)

## Recommendation

Use the transport that matches the traffic pattern.

Recommended split for this repository:

- Telemetry live path: local UDP.
- Back/front control or ordered exchange: local TCP.
- Back/front fan-out or PUB/SUB: ZeroMQ when built with `-DLBR_ENABLE_ZEROMQ=ON`.

Why this split:

- UDP keeps capture low-latency and avoids producer backpressure.
- TCP preserves ordering and delivery when the front side needs a stable stream.
- ZeroMQ gives a cleaner adapter boundary when multiple consumers need the same feed.

Implementation note in this repository:

- `LocalUdpTransport` is available for timeout-based datagram reads.
- `LocalTcpTransport` remains available for ordered stream semantics.
- `LocalZmqTransport` is available for PUB/SUB patterns when built with `-DLBR_ENABLE_ZEROMQ=ON`.

Keep the transport behind a small adapter so the pipeline contract does not change.

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

For binary transport, use protobuf with the schema in
[connector-message.proto](connector-message.proto).

When the flight/embedded side is constrained, use nanopb generated code from the same
`.proto` schema to keep wire compatibility with the ground side.

Required fields:

- `schema_version`: integer, `>= 1`
- `message_type`: non-empty string identifying envelope kind (`telemetry_frame`, `telemetry_decoded`, custom, etc.)
- `sequence`: monotonically increasing integer starting at `0`
- `timestamp_ms`: Unix epoch milliseconds
- `source`: non-empty source identifier (for example `sx1262`, `simulated`, or custom identifiers)
- `payload_b64`: base64-encoded raw telemetry bytes

Optional fields:

- `checksum_hex`: integrity marker in hexadecimal if the producer wants it
- `metadata`: extra implementation-specific fields that do not change the contract

The envelope is intentionally extensible so teams can introduce protocol-specific message types
and source identifiers without changing transport adapters.

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

The ground software can optionally perform a lightweight local interpretation pass for operator visibility,
but this does not change the connector contract and does not replace consumer-side mission interpretation.

## Notes For The Other Team

If they need to change transport later, they should replace only the connector adapter, not the pipeline or radio backends.

The formal schema is in [connector-message.schema.json](connector-message.schema.json).