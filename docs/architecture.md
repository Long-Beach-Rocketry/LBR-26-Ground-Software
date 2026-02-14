# Architecture

## Modules

- `cli::Args` (`include/cli/args.h`, `src/cli/args.cc`)
  - Wraps `argc/argv` as safe argument views.
- `cli::Config` (`include/cli/config.h`, `src/cli/config.cc`)
  - Parses CLI flags.
  - Loads and validates YAML config.
  - Produces `RuntimeSettings`.
- `SDRPipeline` (`include/sdr_pipeline.h`, `src/sdr_pipeline.cc`)
  - Receives validated `RuntimeSettings`.
  - Executes pipeline entry routine (currently minimal logging behavior).
- `main` (`src/main.cc`)
  - Orchestrates startup, parse flow, and pipeline execution.
- test suites (`tests/*.cc`)
  - GoogleTest-based unit suites:
    - `ArgsTests` (`tests/args_tests.cc`)
    - `ConfigTests` (`tests/config_tests.cc`)
    - `PipelineTests` (`tests/pipeline_tests.cc`)

## Runtime Flow

1. `main` builds `cli::Args` from `argc/argv`.
2. `cli::Config::parse` handles:
   - `-h/--help`
   - `-c/--config <file>`
   - `-v/--verbose`
3. When `-c` is used, YAML is parsed and validated.
4. `SDRPipeline` is constructed with `config.settings()`.
5. `SDRPipeline::run` executes.

## Current Status

- Config and startup flow are implemented.
- SDR hardware and DSP stages are not implemented yet.
