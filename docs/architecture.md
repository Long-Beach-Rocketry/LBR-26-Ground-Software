# Architecture

## Modules

- `cli::Args` (`LoRa/include/cli/args.h`, `LoRa/src/cli/args.cc`)
  - wraps `argc/argv` as safe argument views.
- `cli::Config` (`LoRa/include/cli/config.h`, `LoRa/src/cli/config.cc`)
  - parses CLI flags and YAML.
  - validates runtime settings.
- `periph::ILoRaModule` and modules (`LoRa/include/periph/*.h`, `LoRa/src/periph/*.cc`)
  - strategy abstraction used by the pipeline.
  - selectable module implementations: `sx1262`, `sx127`.
- `periph::LoRaModuleFactory` (`LoRa/include/periph/lora_module_factory.h`)
  - factory catalogue mapping configuration names to module creators.
  - centralizes module creation and supported-name validation for the CLI, ground
    executable and HIL runner.
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
4. `periph::LoRaModuleFactory` creates the module implementation selected from runtime settings.
5. `SDRPipeline` is constructed and executed.

## Adding a LoRa module

To add another board/radio implementation:

1. Implement `periph::ILoRaModule` in `LoRa/include/periph` and `LoRa/src/periph`.
2. Add one creator entry to the catalogue in `lora_module_factory.cc`.
3. Add the source file to the `lora_periph` target in `LoRa/CMakeLists.txt`.

The CLI validation, module construction, HIL runner and connector source identifier
then use the new name automatically. The pipeline remains independent of the concrete
module class.

## Current Status

- `virtual` LoRa mode is supported and used in CI.
- `hardware` mode is reserved for real radio drivers and fails fast when not implemented.
- connector and schema plumbing are in place for backend/frontend integration.
