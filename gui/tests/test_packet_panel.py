"""
@File:   test_packet_panel.py
@Brief:  Unit tests for packet panel formatting helpers
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry
"""

import sys
import unittest
from pathlib import Path
from types import SimpleNamespace

ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from gui.ui.widgets.packet_panel_formatting import format_packet_view, get_field_unit, raw_value


class PacketPanelFormattingTests(unittest.TestCase):
    def test_nested_packet_shape_is_formatted(self):
        packet = SimpleNamespace(
            packet_id=17,
            time=SimpleNamespace(unix_ms=123456789, iso_time="2026-04-30T12:34:56Z"),
            data=SimpleNamespace(
                altitude_m=16.0,
                velocity_mps=18.7,
                acceleration_mps2=-9.8,
                temperature_c=21.5,
                battery_v=8.3,
                pressure_pa=101325.0,
                signal_dbm=-61.0,
                packet_count=4,
            ),
            status=SimpleNamespace(
                connection_state="Connected",
                device="rtlsdr",
                packets_received=12,
                packets_lost=1,
                uptime_seconds=42.4,
                status_text="ok",
            ),
            messages=[
                SimpleNamespace(level="info", text="armed"),
                SimpleNamespace(level="warn", text="low battery"),
            ],
        )

        values = format_packet_view(packet)

        self.assertEqual(values["packet_id"], "17")
        self.assertEqual(values["unix_ms"], "123456789")
        self.assertEqual(values["iso_time"], "2026-04-30T12:34:56Z")
        self.assertEqual(values["altitude"], "16.00 m")
        self.assertEqual(values["velocity"], "18.70 m/s")
        self.assertEqual(values["accel"], "-9.80 m/s²")
        self.assertEqual(values["temp"], "21.50 C")
        self.assertEqual(values["battery"], "8.30 V")
        self.assertEqual(values["pressure"], "101325.00 Pa")
        self.assertEqual(values["signal"], "-61.00 dBm")
        self.assertEqual(values["pkt_count"], "4")
        self.assertEqual(values["conn_state"], "Connected")
        self.assertEqual(values["device"], "rtlsdr")
        self.assertEqual(values["pkts_rx"], "12")
        self.assertEqual(values["pkts_lost"], "1")
        self.assertEqual(values["uptime"], "42.4 s")
        self.assertEqual(values["status_text"], "ok")
        self.assertEqual(values["messages"], "[info] armed\n[warn] low battery")

    def test_direct_gui_frame_shape_is_also_supported(self):
        packet = SimpleNamespace(
            packet_id=5,
            unix_ms=99,
            iso_time="2026-04-30T00:00:00Z",
            altitude=12.4,
            velocity=18.7,
            acceleration=31.2,
            temperature=22.5,
            battery=8.32,
            pressure_pa=101325.0,
            signal=-61.0,
            packet_count=1,
        )

        values = format_packet_view(packet)

        self.assertEqual(values["packet_id"], "5")
        self.assertEqual(values["unix_ms"], "99")
        self.assertEqual(values["iso_time"], "2026-04-30T00:00:00Z")
        self.assertEqual(values["altitude"], "12.40 m")
        self.assertEqual(values["velocity"], "18.70 m/s")
        self.assertEqual(values["accel"], "31.20 m/s²")
        self.assertEqual(values["temp"], "22.50 C")
        self.assertEqual(values["battery"], "8.32 V")
        self.assertEqual(values["pressure"], "101325.00 Pa")
        self.assertEqual(values["signal"], "-61.00 dBm")
        self.assertEqual(values["pkt_count"], "1")
        self.assertEqual(values["conn_state"], "-")
        self.assertEqual(values["device"], "-")
        self.assertEqual(values["messages"], "")

    def test_missing_values_fall_back_to_dash(self):
        values = format_packet_view(SimpleNamespace())

        self.assertEqual(values["packet_id"], "-")
        self.assertEqual(values["unix_ms"], "-")
        self.assertEqual(values["iso_time"], "-")
        self.assertEqual(values["altitude"], "-")
        self.assertEqual(values["messages"], "")

    def test_get_field_unit_extracts_units_from_specs(self):
        self.assertEqual(get_field_unit("altitude"), "m")
        self.assertEqual(get_field_unit("velocity"), "m/s")
        self.assertEqual(get_field_unit("accel"), "m/s²")
        self.assertEqual(get_field_unit("signal"), "dBm")

    def test_get_field_unit_supports_fallback(self):
        self.assertEqual(get_field_unit("pkt_count"), "-")
        self.assertEqual(get_field_unit("pkt_count", "count"), "count")
        self.assertEqual(get_field_unit("missing_field", "unit"), "unit")

    def test_raw_value_supports_nested_and_flat_shapes(self):
        nested = SimpleNamespace(
            data=SimpleNamespace(
                altitude_m=123.0,
                velocity_mps=45.0,
                packet_count=9,
            )
        )
        flat = SimpleNamespace(
            altitude=12.4,
            velocity=18.7,
            packet_count=3,
        )

        self.assertEqual(raw_value(nested, "altitude"), 123.0)
        self.assertEqual(raw_value(nested, "velocity"), 45.0)
        self.assertEqual(raw_value(nested, "pkt_count"), 9)

        self.assertEqual(raw_value(flat, "altitude"), 12.4)
        self.assertEqual(raw_value(flat, "velocity"), 18.7)
        self.assertEqual(raw_value(flat, "pkt_count"), 3)


if __name__ == "__main__":
    unittest.main()
