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
  output_path: "output/frame.bin"
```

## Sections

### `sdr`

- `device` (`string`)
- `sample_rate_hz` (`int`)
- `center_freq_hz` (`int`)
- `gain_db` (`int`)

### `pipeline`

- `verbose` (`bool`)
- `output_path` (`string`)

## Defaults

If a key is missing, defaults are used:

- `sdr.device = "rtlsdr"`
- `sdr.sample_rate_hz = 2048000`
- `sdr.center_freq_hz = 433920000`
- `sdr.gain_db = 30`
- `pipeline.verbose = false`
- `pipeline.output_path = "output/frame.bin"`

## Validation Rules

- `sdr.sample_rate_hz > 0`
- `sdr.center_freq_hz > 0`
- `pipeline.output_path` must not be empty

If validation fails, the app exits with an error.

## CLI Override

- `-v` overrides YAML and forces `pipeline.verbose = true`.
