"""
@File:   telemetry_cards.py
@Brief:  Telemetry metric cards widget
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Metric-card UI panel that displays live telemetry values and alerts.
"""

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QFrame,
    QGridLayout,
    QHBoxLayout,
    QLabel,
    QSizePolicy,
    QVBoxLayout,
    QWidget,
)

from models.models import TelemetryFrame
from .packet_panel_formatting import raw_value, get_field_unit
from .alert_rules import default_alert_rules

class MetricCard(QFrame):
    def __init__(self, label, unit, parent = None):
        super().__init__(parent)

        self.setObjectName("MetricCard")
        self.setFrameShape(QFrame.Shape.StyledPanel)
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        self.setMinimumWidth(140)

        layout = QVBoxLayout(self)
        layout.setContentsMargins(14, 10, 14, 10)
        layout.setSpacing(2)

        self._label = QLabel(label.upper())
        self._label.setObjectName("CardLabel")

        self._value = QLabel("-")
        self._value.setObjectName("CardValue")

        self._unit_label = QLabel(unit)
        self._unit_label.setObjectName("CardUnit")

        row = QHBoxLayout()
        row.setSpacing(4)
        row.addWidget(self._value)
        row.addWidget(self._unit_label)
        row.addStretch()

        layout.addWidget(self._label)
        layout.addLayout(row)

        self._apply_normal_style()

    def set_value(self, value, decimals = 1):
        if value is None:
            self._value.setText("-")
            return

        try:
            self._value.setText(f"{value:.{decimals}f}")
        except (TypeError, ValueError):
            self._value.setText(str(value))

    def set_alert(self, is_alert):
        if is_alert:
            self._value.setStyleSheet(
                "font-size: 22px; font-weight: 600; color: #dc2626;"
                "font-family: 'Courier New', monospace;"
            )
            self.setStyleSheet("""
                QFrame#MetricCard {
                    background: #fff5f5;
                    border: 1.5px solid #fca5a5;
                    border-radius: 8px;
                }
            """)
        else:
            self._apply_normal_style()

    def _apply_normal_style(self):
        self.setStyleSheet("""
            QFrame#MetricCard {
                background: #ffffff;
                border: 1.5px solid #e2e8f0;
                border-radius: 8px;
            }
            QFrame#MetricCard:hover {
                border-color: #94a3b8;
            }
        """)
        self._label.setStyleSheet(
            "font-size: 10px; font-weight: 600; color: #94a3b8;"
            "letter-spacing: 1px; font-family: 'Courier New', monospace;"
        )
        self._value.setStyleSheet(
            "font-size: 22px; font-weight: 600; color: #0f172a;"
            "font-family: 'Courier New', monospace;"
        )
        self._unit_label.setStyleSheet(
            "font-size: 12px; color: #64748b;"
            "font-family: 'Courier New', monospace; margin-top: 8px;"
        )

class SectionHeader(QLabel):
    def __init__(self, text, parent = None):
        super().__init__(text.upper(), parent)
        self.setStyleSheet(
            "font-size: 10px; font-weight: 700; color: #64748b;"
            "letter-spacing: 2px; font-family: 'Courier New', monospace;"
            "padding: 8px 0 4px 0;"
        )
    
class TelemetryCardsPanel(QWidget):
    def __init__(self, parent = None, alert_rules = None):
        super().__init__(parent)
        self._alert_rules = alert_rules or default_alert_rules()
        self._build_ui()

    def _is_alert(self, metric_key, value):
        rule = self._alert_rules.get(metric_key)
        if rule is None:
            return False
        return rule.is_alert(value)
    
    def _build_ui(self):
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        root.addWidget(SectionHeader("Flight Dynamics"))

        flight_grid = QGridLayout()
        flight_grid.setSpacing(10)

        self._alt = MetricCard("Altitude", get_field_unit("altitude"))
        self._vel = MetricCard("Velocity", get_field_unit("velocity"))
        self._accel = MetricCard("Acceleration", get_field_unit("accel"))

        flight_grid.addWidget(self._alt, 0, 0)
        flight_grid.addWidget(self._vel, 0, 1)
        flight_grid.addWidget(self._accel, 0, 2)
        root.addLayout(flight_grid)

        root.addWidget(SectionHeader("Onboard Systems"))

        sys_grid = QGridLayout()
        sys_grid.setSpacing(10)

        self._temp = MetricCard("Temperature", get_field_unit("temp"))
        self._battery = MetricCard("Battery", get_field_unit("battery"))
        self._pressure = MetricCard("Pressure", get_field_unit("pressure"))

        sys_grid.addWidget(self._temp, 0, 0)
        sys_grid.addWidget(self._battery, 0, 1)
        sys_grid.addWidget(self._pressure, 0, 2)
        root.addLayout(sys_grid)

        root.addWidget(SectionHeader("RF Link"))

        rf_grid = QGridLayout()
        rf_grid.setSpacing(10)

        self._rssi = MetricCard("RSSI", get_field_unit("signal"))
        self._pkt_cnt = MetricCard("Packets", get_field_unit("pkt_count", "count"))

        rf_grid.addWidget(self._rssi, 0, 0)
        rf_grid.addWidget(self._pkt_cnt, 0, 1)
        rf_grid.addWidget(QWidget(), 0, 2)
        root.addLayout(rf_grid)

        root.addStretch()

    def update_frame(self, frame: TelemetryFrame):
        alt = raw_value(frame, "altitude")
        vel = raw_value(frame, "velocity")
        accel = raw_value(frame, "accel")

        temp = raw_value(frame, "temp")
        battery = raw_value(frame, "battery")
        pressure = raw_value(frame, "pressure")

        signal = raw_value(frame, "signal")
        pkt_cnt = raw_value(frame, "pkt_count")

        self._alt.set_value(alt, 1)
        self._vel.set_value(vel, 1)
        self._accel.set_value(accel, 2)

        self._temp.set_value(temp, 1)
        self._temp.set_alert(self._is_alert("temp", temp))

        self._battery.set_value(battery, 2)
        self._battery.set_alert(self._is_alert("battery", battery))

        self._pressure.set_value(pressure, 0)
        self._pressure.set_alert(self._is_alert("pressure", pressure))

        self._rssi.set_value(signal, 1)
        self._rssi.set_alert(self._is_alert("signal", signal))

        self._pkt_cnt.set_value(float(pkt_cnt) if pkt_cnt is not None else None, 0)