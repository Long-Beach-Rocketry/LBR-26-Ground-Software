"""
@File:   packet_panel_formatting.py
@Brief:  Pure formatting helpers for the packet viewer panel
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Helper functions that map telemetry packet objects into display strings.
    Kept free of GUI dependencies so it can be unit tested without PySide6.
"""

_DATA_FIELD_SPECS = [
    ("altitude", ("altitude_m", "altitude"), "{:.2f} m"),
    ("velocity", ("velocity_mps", "velocity"), "{:.2f} m/s"),
    ("accel", ("acceleration_mps2", "acceleration"), "{:.2f} m/s²"),
    ("temp", ("temperature_c", "temperature"), "{:.2f} C"),
    ("battery", ("battery_v", "battery"), "{:.2f} V"),
    ("pressure", ("pressure_pa",), "{:.2f} Pa"),
    ("signal", ("signal_dbm", "signal"), "{:.2f} dBm"),
    ("pkt_count", ("packet_count",), "{}"),
]


def _first_attr(obj, names, default=None):
    if obj is None:
        return default

    for name in names:
        if hasattr(obj, name):
            value = getattr(obj, name)
            if value is not None:
                return value

    return default


def _format_value(value, pattern):
    if value is None:
        return "-"

    try:
        return pattern.format(value)
    except (TypeError, ValueError):
        return str(value)


def _format_messages(messages):
    lines = []
    for message in messages or []:
        level = _first_attr(message, ("level",), "-")
        text = _first_attr(message, ("text",), "")
        lines.append(f"[{level}] {text}")

    return "\n".join(lines)


def format_packet_view(packet):
    data = _first_attr(packet, ("data",), packet)
    time = _first_attr(packet, ("time",), None)
    status = _first_attr(packet, ("status",), None)

    values = {
        "packet_id": str(_first_attr(packet, ("packet_id",), "-")),
        "unix_ms": str(_first_attr(time, ("unix_ms",), _first_attr(packet, ("unix_ms",), "-"))),
        "iso_time": _first_attr(time, ("iso_time",), _first_attr(packet, ("iso_time",), "-")),
        "conn_state": str(_first_attr(status, ("connection_state",), "-")),
        "device": _first_attr(status, ("device",), "-"),
        "pkts_rx": str(_first_attr(status, ("packets_received",), "-")),
        "pkts_lost": str(_first_attr(status, ("packets_lost",), "-")),
        "uptime": _format_value(_first_attr(status, ("uptime_seconds",), None), "{:.1f} s"),
        "status_text": _first_attr(status, ("status_text",), "-"),
        "messages": _format_messages(_first_attr(packet, ("messages",), [])),
    }

    for field_name, aliases, pattern in _DATA_FIELD_SPECS:
        values[field_name] = _format_value(_first_attr(data, aliases, None), pattern)

    return values


def get_field_spec(field_key):
    """Return the full spec tuple (name, aliases, pattern) for a named field.
    
    Used to extract unit and formatting info without duplicating specs.
    Returns None if field_key is not found.
    """
    for fname, aliases, pattern in _DATA_FIELD_SPECS:
        if fname == field_key:
            return (fname, aliases, pattern)
    return None


def get_field_unit(field_key, fallback=None):
    """Extract the unit string from the format pattern for a named field.
    
    Examples:
        get_field_unit("altitude")  -> "m"
        get_field_unit("velocity")  -> "m/s"
        get_field_unit("temp")      -> "C"
        get_field_unit("pkt_count") -> "-" (or custom fallback)
    
    Args:
        field_key: Name of the field (e.g. "altitude", "velocity")
        fallback: Optional unit string to use if no unit found in pattern.
                 Defaults to "-" if not specified.
    
    Returns the unit string, fallback value, or "-" if nothing found.
    """
    spec = get_field_spec(field_key)
    if not spec:
        return fallback if fallback is not None else "-"
    
    _, _, pattern = spec
    # Extract unit from pattern like "{:.2f} m" -> "m"
    # Simple heuristic: look for non-format-string content after }
    pattern_str = str(pattern)
    parts = pattern_str.split("}")
    if len(parts) > 1:
        unit = parts[1].strip()
        if unit:
            return unit
    
    # No unit found in pattern, use fallback
    return fallback if fallback is not None else "-"


def raw_value(packet_or_frame, field_key):
    """Return the raw (unformatted) value for a named data field.

    Works for both nested packet objects (with `data`) and flat frame
    objects. Field names correspond to the keys used in
    `_DATA_FIELD_SPECS` (e.g. 'altitude', 'velocity', 'pkt_count').

    Returns None when no value is present.
    """
    data = _first_attr(packet_or_frame, ("data",), packet_or_frame)

    for fname, aliases, _ in _DATA_FIELD_SPECS:
        if fname == field_key:
            return _first_attr(data, aliases, None)

    # fallback: try direct attribute on the object
    return _first_attr(packet_or_frame, (field_key,), None)
