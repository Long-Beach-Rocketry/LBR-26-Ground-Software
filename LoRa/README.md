# LoRa Ground Software

Ground software skeleton for the Long Beach Rocketry station, organized under the LoRa root.

## What This Update Adds

- A hardware abstraction interface: `periph::ILoRaModule`
- Two interchangeable backend skeletons:
  - `periph::SX1262Module`
  - `periph::SX127Module`
- `SDRPipeline` now depends on the LoRa abstraction instead of concrete hardware
- CLI/config support to choose LoRa module at runtime (`sx1262` or `sx127`)

## Why The Abstraction Helps

The pipeline now consumes telemetry through `ILoRaModule`:

- `init() -> LoRaStatusCode`
- `transmit(const uint8_t* buf, size_t len) -> LoRaTransmitResult`
- `receive(uint8_t* buf, size_t max_len, uint32_t timeout_ms) -> LoRaReceiveResult`

Because `SDRPipeline` only sees this interface, switching from SX126x to SX127x does not require pipeline logic changes. The only change is module selection in config/CLI.

## Folder Layout

```text
LoRa/
  include/
Keyword interface (recommended):

```powershell
.\dev.ps1 build
```
    cli/
Legacy direct entrypoint:
      config.h
    periph/
      i_lora_module.h
      sx1262_module.h
      sx127_module.h
    sdr_pipeline.h
  src/
    cli/
.\dev.ps1 build-only
      config.cc
    periph/
    pipeline_tests.cc
  config.demo.yaml
  CMakeLists.txt
```

## Module Selection

Choose LoRa backend with either:

- CLI: `--lora-module sx1262` or `--lora-module sx127`
- YAML:

```yaml
lora:
  module: "sx1262"
```

The CLI option overrides defaults and is validated.

## Build And Run

From repository root:

Keyword interface (recommended):

```powershell
.\dev.ps1 build
```

Build only (skip tests):

```powershell
.\dev.ps1 build-only
```

Sanity checks (when `clang-format` and `clang-tidy` are available in `PATH`):

```powershell
.\dev.ps1 sanity
```

If either tool is missing from `PATH`, `sanity-check` fails with a clear message.

Generate API documentation (when `doxygen` is available in `PATH`):

```powershell
.\dev.ps1 docs
```

Generate PDF API documentation:

```powershell
.\dev.ps1 docs-pdf
```

Generated HTML entry point:

- `build/docs/doxygen/html/index.html`

Generated PDF:

- `build/docs/doxygen/latex/refman.pdf`

Run app:

```powershell
.\build\lbr_ground.exe --help
.\build\lbr_ground.exe -c .\LoRa\config.demo.yaml -v
.\build\lbr_ground.exe --lora-module sx127 -v
```

## Coverage

Current score (last run):

- Project source aggregate: line 86.65%, functions 100.00%, branches 53.13%

Project source metrics (all production .cc files):

| File | Line | Functions | Branches |
| --- | ---: | ---: | ---: |
| args.cc | 90.91% | 100.00% | 78.57% |
| config.cc | 100.00% | 100.00% | 65.13% |
| message.cc | 78.98% | 100.00% | 46.05% |
| ndjson_file.cc | 93.75% | 100.00% | 53.85% |
| local_tcp_transport.cc | 78.72% | 100.00% | 42.45% |
| sx1262_module.cc | 100.00% | 100.00% | 0.00% |
| sx127_module.cc | 100.00% | 100.00% | 0.00% |
| sdr_pipeline.cc | 100.00% | 100.00% | 71.43% |
| main.cc | 89.47% | 100.00% | 43.33% |
| Project aggregate | 86.65% | 100.00% | 53.13% |

Generate coverage from repository root:

```powershell
.\dev.ps1 coverage
```

Coverage artifacts are written to:

- [build-coverage/coverage](../build-coverage/coverage)

Main project source reports are available in:

- [build-coverage/coverage/args.cc.gcov](../build-coverage/coverage/args.cc.gcov)
- [build-coverage/coverage/config.cc.gcov](../build-coverage/coverage/config.cc.gcov)
- [build-coverage/coverage/sdr_pipeline.cc.gcov](../build-coverage/coverage/sdr_pipeline.cc.gcov)
- [build-coverage/coverage/main.cc.gcov](../build-coverage/coverage/main.cc.gcov)

## Current Backend State

`SX1262Module` and `SX127Module` are intentionally skeletal backends. They compile and integrate with the pipeline contract but still need hardware-specific SPI/GPIO/radio register logic for production telemetry.

## Connector

The connector contract used between the LoRa side and the consumer side is documented in [docs/connector.md](docs/connector.md).

The implementation lives under [include/connector](include/connector) and [src/connector](src/connector).

Short version:

- Preferred transport: local socket for live data exchange.
- Fallback transport: file-based replay for offline testing and debugging.
- The transport layer is intentionally separate from the LoRa module abstraction so the other team can implement it without changing the pipeline contract.
