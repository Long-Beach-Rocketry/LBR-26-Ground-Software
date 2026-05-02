"""
@File:   test_telemetry_cards.py
@Brief:  Unit tests for telemetry cards widget
@Author: Luis Fernandes
@Orgin:  Long Beach Rocketry
"""

import os
import sys
import unittest
from pathlib import Path

# Keep Qt tests headless-friendly for CI/local terminals.
os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

GUI_ROOT = ROOT / "gui"
if str(GUI_ROOT) not in sys.path:
    sys.path.insert(0, str(GUI_ROOT))

from PySide6.QtWidgets import QApplication

from gui.models.models import TelemetryFrame
from gui.ui.widgets.telemetry_cards import TelemetryCardsPanel
from gui.ui.widgets.alert_rules import NeverAlertRule


class TelemetryCardsPanelTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls._app = QApplication.instance() or QApplication([])

    def test_units_are_loaded_from_field_specs(self):
        panel = TelemetryCardsPanel()

        self.assertEqual(panel._alt._unit_label.text(), "m")
        self.assertEqual(panel._vel._unit_label.text(), "m/s")
        self.assertEqual(panel._accel._unit_label.text(), "m/s²")
        self.assertEqual(panel._temp._unit_label.text(), "C")
        self.assertEqual(panel._battery._unit_label.text(), "V")
        self.assertEqual(panel._pressure._unit_label.text(), "Pa")
        self.assertEqual(panel._rssi._unit_label.text(), "dBm")
        self.assertEqual(panel._pkt_cnt._unit_label.text(), "count")

    def test_update_frame_sets_values_and_alerts(self):
        panel = TelemetryCardsPanel()

        alert_frame = TelemetryFrame(
            altitude=123.4,
            velocity=67.8,
            acceleration=1.23,
            temperature=85.0,
            battery=4.4,
            pressure_pa=45000.0,
            signal=-100.0,
            packet_count=7,
        )
        panel.update_frame(alert_frame)

        self.assertEqual(panel._alt._value.text(), "123.4")
        self.assertEqual(panel._vel._value.text(), "67.8")
        self.assertEqual(panel._accel._value.text(), "1.23")
        self.assertEqual(panel._temp._value.text(), "85.0")
        self.assertEqual(panel._battery._value.text(), "4.40")
        self.assertEqual(panel._pressure._value.text(), "45000")
        self.assertEqual(panel._rssi._value.text(), "-100.0")
        self.assertEqual(panel._pkt_cnt._value.text(), "7")

        self.assertIn("#dc2626", panel._temp._value.styleSheet())
        self.assertIn("#dc2626", panel._battery._value.styleSheet())
        self.assertIn("#dc2626", panel._pressure._value.styleSheet())
        self.assertIn("#dc2626", panel._rssi._value.styleSheet())

        nominal_frame = TelemetryFrame(
            altitude=130.0,
            velocity=55.0,
            acceleration=0.5,
            temperature=25.0,
            battery=8.2,
            pressure_pa=101325.0,
            signal=-60.0,
            packet_count=8,
        )
        panel.update_frame(nominal_frame)

        self.assertNotIn("#dc2626", panel._temp._value.styleSheet())
        self.assertNotIn("#dc2626", panel._battery._value.styleSheet())
        self.assertNotIn("#dc2626", panel._pressure._value.styleSheet())
        self.assertNotIn("#dc2626", panel._rssi._value.styleSheet())

    def test_custom_alert_strategy_can_disable_alerts(self):
        custom_rules = {
            "temp": NeverAlertRule(),
            "battery": NeverAlertRule(),
            "pressure": NeverAlertRule(),
            "signal": NeverAlertRule(),
        }
        panel = TelemetryCardsPanel(alert_rules=custom_rules)

        frame = TelemetryFrame(
            temperature=120.0,
            battery=1.0,
            pressure_pa=1000.0,
            signal=-120.0,
        )
        panel.update_frame(frame)

        self.assertNotIn("#dc2626", panel._temp._value.styleSheet())
        self.assertNotIn("#dc2626", panel._battery._value.styleSheet())
        self.assertNotIn("#dc2626", panel._pressure._value.styleSheet())
        self.assertNotIn("#dc2626", panel._rssi._value.styleSheet())


if __name__ == "__main__":
    unittest.main()
