# LBR-26-Ground-Software

Ground software skeleton for the Long Beach Rocketry SDR station.

## Current Scope

Implemented in this version:
- CLI argument container (`cli::Args`)
- CLI parser/config (`cli::Config`)
- Minimal pipeline shell (`SDRPipeline`)
- CMake build targets (`cli`, `sdr_pipeline`, `lbr_ground`)

Not implemented yet:
- SDR device integration
- DSP chain and decoding logic
- Metrics and telemetry persistence

## Project Structure

```text
include/
  cli/
    args.h
    config.h
  sdr_pipeline.h
src/
  cli/
    args.cc
    config.cc
  main.cc
  sdr_pipeline.cc
config.demo.yaml
CMakeLists.txt
README.md
```

## CLI Options

Implemented options:
- `-h`, `--help`: print usage and exit successfully
- `-c`, `--config <file>`: set config file path
- `-v`, `--verbose`: enable verbose mode

Unknown options or missing `-c` value return an error and usage text.

## Run

From the repo root:
```powershell
.\build\lbr_ground.exe --help
.\build\lbr_ground.exe -v
.\build\lbr_ground.exe -c .\config.yaml -v
.\build\lbr_ground.exe -c .\config.demo.yaml -v
```

## Code Flow

1. `main` wraps raw CLI input into `cli::Args`
2. `cli::Config::parse` validates options and returns `ParseStatus`
3. `SDRPipeline` is created with validated config
4. `SDRPipeline::run` executes the pipeline entry routine
