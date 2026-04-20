# Configuration

Configuration is read from a YAML file passed with:

```text
-c <file>
```

## Example

```yaml
sdr:
  device: "rtlsdr"
  sample_rate_hz: 2048000
  center_freq_hz: 433920000
  gain_db: 30

pipeline:
  verbose: true
  interpret_telemetry: true
  output_path: "output/frame.bin"

lora:
  module: "sx1262"
  mode: "virtual"
```

## Sections

### `sdr`

- `device` (`string`)
- `sample_rate_hz` (`int`)
- `center_freq_hz` (`int`)
- `gain_db` (`int`)

### `pipeline`

- `verbose` (`bool`)
- `interpret_telemetry` (`bool`)
- `output_path` (`string`)

### `lora`

- `module` (`string`): `sx1262` or `sx127`
- `mode` (`string`): `virtual` or `hardware`

## Defaults

If a key is missing, defaults are used:

- `sdr.device = "rtlsdr"`
- `sdr.sample_rate_hz = 2048000`
- `sdr.center_freq_hz = 433920000`
- `sdr.gain_db = 30`
- `pipeline.verbose = false`
- `pipeline.interpret_telemetry = true`
- `pipeline.output_path = "output/frame.bin"`
- `lora.module = "sx1262"`
- `lora.mode = "virtual"`

## Validation Rules

- `sdr.sample_rate_hz > 0`
- `sdr.center_freq_hz > 0`
- `pipeline.output_path` must not be empty
- `lora.module` must be one of: `sx1262`, `sx127`
- `lora.mode` must be one of: `virtual`, `hardware`

If validation fails, the app exits with an error.

## CLI Override

- `-v` overrides YAML and forces `pipeline.verbose = true`.
- `-m, --lora-module <name>` overrides `lora.module`.
- `-b, --lora-mode <name>` overrides `lora.mode`.
