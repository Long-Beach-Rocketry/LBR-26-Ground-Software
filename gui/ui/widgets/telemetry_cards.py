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
        self._value.setText(f"{value:.{decimals}f}")

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
    def __init__(self, parent = None):
        super().__init__(parent)
        self._build_ui()
    
    def _build_ui(self):
        root = QVBoxLayout(self)
        root.setContentsMargins(0, 0, 0, 0)
        root.setSpacing(0)

        root.addWidget(SectionHeader("Flight Dynamics"))

        flight_grid = QGridLayout()
        flight_grid.setSpacing(10)

        self._alt = MetricCard("Altitude", "m")
        self._vel = MetricCard("Velocity", "m/s")
        self._accel = MetricCard("Acceleration", "m/s")

        flight_grid.addWidget(self._alt, 0, 0)
        flight_grid.addWidget(self._vel, 0, 1)
        flight_grid.addWidget(self._accel, 0, 2)
        root.addLayout(flight_grid)

        root.addWidget(SectionHeader("Onboard Systems"))

        sys_grid = QGridLayout()
        sys_grid.setSpacing(10)

        self._temp = MetricCard("Temperature", "C")
        self._battery = MetricCard("Battery", "V")
        self._pressure = MetricCard("Pressure", "Pa")

        sys_grid.addWidget(self._temp, 0, 0)
        sys_grid.addWidget(self._battery, 0, 1)
        sys_grid.addWidget(self._pressure, 0, 2)
        root.addLayout(sys_grid)

        root.addWidget(SectionHeader("RF Link"))

        rf_grid = QGridLayout()
        rf_grid.setSpacing(10)

        self._rssi = MetricCard("RSSI", "dBm")
        self._pkt_cnt = MetricCard("Packets", "rx")

        rf_grid.addWidget(self._rssi, 0, 0)
        rf_grid.addWidget(self._pkt_cnt, 0, 1)
        rf_grid.addWidget(QWidget(), 0, 2)
        root.addLayout(rf_grid)

        root.addStretch()

    def update_frame(self, frame: TelemetryFrame):
        self._alt.set_value(frame.altitude, 1)
        self._vel.set_value(frame.velocity, 1)
        self._accel.set_value(frame.acceleration, 2)

        self._temp.set_value(frame.temperature, 1)
        self._temp.set_alert(frame.temperature > 80)

        self._battery.set_value(frame.battery, 2)
        self._battery.set_alert(frame.battery < 5.0)

        self._pressure.set_value(frame.pressure_pa, 0)
        self._pressure.set_alert(frame.pressure_pa < 50000)

        self._rssi.set_value(frame.signal, 1)
        self._rssi.set_alert(frame.signal < -90)

        self._pkt_cnt.set_value(float(frame.packet_count), 0)