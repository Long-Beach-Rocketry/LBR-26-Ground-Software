# Architecture

## Modules

- `cli::Args` (`LoRa/include/cli/args.h`, `LoRa/src/cli/args.cc`)
  - wraps `argc/argv` as safe argument views.
- `cli::Config` (`LoRa/include/cli/config.h`, `LoRa/src/cli/config.cc`)
  - parses CLI flags and YAML.
  - validates runtime settings.
- `periph::ILoRaModule` and modules (`LoRa/include/periph/*.h`, `LoRa/src/periph/*.cc`)
  - abstraction used by pipeline.
  - selectable module implementations: `sx1262`, `sx127`.
- `SDRPipeline` (`LoRa/include/sdr_pipeline.h`, `LoRa/src/sdr_pipeline.cc`)
  - orchestrates the runtime data path from module to output.
- connector transports (`LoRa/include/connector/*.h`, `LoRa/src/connector/*.cc`)
  - local TCP
  - local UDP
  - local ZeroMQ (optional, compile-time feature)
  - NDJSON replay file transport
- executables
  - `lbr_ground` (`LoRa/src/main.cc`)
  - `lbr_hil_runner` (`LoRa/src/hil_runner.cc`)
- tests (`LoRa/tests/*.cc`)
  - config, args, connector, pipeline coverage.

## Runtime Flow

1. `lbr_ground` builds `cli::Args` from `argc/argv`.
2. `cli::Config::parse` handles:
   - `-h/--help`
   - `-c/--config <file>`
  - `-m/--lora-module <sx1262|sx127>`
  - `-b/--lora-mode <virtual|hardware>`
   - `-v/--verbose`
3. YAML config is parsed and validated when provided.
4. LoRa module implementation is selected from runtime settings.
5. `SDRPipeline` is constructed and executed.

## Current Status

- `virtual` LoRa mode is supported and used in CI.
- `hardware` mode is reserved for real radio drivers and fails fast when not implemented.
- connector and schema plumbing are in place for backend/frontend integration.
