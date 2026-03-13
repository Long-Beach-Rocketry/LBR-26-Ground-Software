from PySide6.QtCore import Property, QEasingCurve, QPropertyAnimation, Qt
from PySide6.QtGui import QColor, QPainter
from PySide6.QtWidgets import QFrame, QHBoxLayout, QLabel, QSizePolicy, QWidget

from gui.models.models import ConnectionState, SystemStatus

class HeartbeatDot(QWidget):
    def __init__(self, parent = None):
        super().__init__(parent)

        self.setFixedSize(14, 14)

        self._color = QColor("#9ca3af")

        self._opacity = 1.0

        self._animation = QPropertyAnimation(self, b"dot_opacity")
        self._animation.setDuration(900)
        self._animation.setStartValue(1.0)
        self._animation.setKeyValueAt(0.5, 0.2)
        self._animation.setEndValue(1.0)
        self._animation.setEasingCurve(QEasingCurve.Type.InOutSine)
        self._animation.setLoopCount(-1)

    def _get_opacity(self):
        return self._opacity
    
    def _set_opacity(self, value):
        self._opacity = value
        self.update()

    dot_opacity = Property(float, _get_opacity, _set_opacity)

    def set_connected(self, connected):
        if connected:
            self._color = QColor("#22c55e")
            self._animation.start()
        else:
            self._animation.stop()
            self._opacity = 1.0
            self._color = QColor("#9ca3af")
            self.update()

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.Antialiasing)
        painter.setOpacity(self._opacity)
        painter.setPen(Qt.PenStyle.NoPen)
        painter.setBrush(self._color)
        margin = 2
        painter.drawEllipse(
            margin, margin,
            self.width() - margin * 2,
            self.height() - margin * 2,
        )

class StatusBar(QFrame):
    def __init__(self, parent = None):
        super().__init__(parent)

        self.setFrameShape(QFrame.Shape.StyledPanel)
        self.setObjectName("StatusBar")
        self.setFixedHeight(44)
        self.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)

        self._build_ui()
        self._apply_style()

    def _build_ui(self):
        layout = QHBoxLayout(self)
        layout.setContentsMargins(16, 0, 16, 0)
        layout.setSpacing(12)

        self._dot = HeartbeatDot()
        layout.addWidget(self._dot)

        self._state_label = QLabel("Disconnected")
        self._state_label.setObjectName("StateLabel")
        layout.addWidget(self._state_label)

        self._add_separator(layout)

        self._sdr_label = QLabel("SDR: -")
        layout.addWidget(self._sdr_label)

        self._freq_label = QLabel("Freq: -")
        layout.addWidget(self._freq_label)

        self._add_separator(layout)

        self._rate_label = QLabel("SR: -")
        layout.addWidget(self._rate_label)

        layout.addStretch()

        self._packets_label = QLabel("Packets: 0")
        layout.addWidget(self._packets_label)

        self._add_separator(layout)

        self._uptime_label = QLabel("Uptime: 0s")
        layout.addWidget(self._uptime_label)

    def _add_separator(self, layout):
        sep = QLabel("|")
        sep.setObjectName("Separator")
        layout.addWidget(sep)

    def _apply_style(self):
        self.setStyleSheet("""
            QFrame#StatusBar {
                background-color: #f8fafc;
                border-bottom: 1px solid #e2e8f0;
            }
            QLabel {
                font-family: 'Courier New', monospace;
                font-size: 11px;
                color: #64748b;
            }
            QLabel#StateLabel {
                font-weight: bold;
                font-size: 11px;
                color: #94a3b8;
                letter-spacing: 1px;
            }
            QLabel#Separator {
                color: #cbd5e1;
            }
        """)

    def update_status(self, status: SystemStatus):
        connected = (status.connection_state == ConnectionState.Connected)

        self._dot.set_connected(connected)

        self._state_label.setText(status.connection_state.name)

        if connected:
            color = "#16a34a"
        elif status.connection_state == ConnectionState.Error:
            color = "#dc2626"
        else:
            color = "#94a3b8"
        
        self._state_label.setStyleSheet(
            f"font-weight: bold; font-size: 11px; color: {color}; latter-spacing: 1px;"
        )

        if status.sdr_config is not None:
            cfg = status.sdr_config
            self._sdr_label.setText(f"SDR: {cfg.device.upper()}")
            self._freq_label.setText(f"Freq: {cfg.center_freq_mhz:.1f} MHz")
            self._rate_label.setText(f"SR: {cfg.sample_rate_mhz:.1f} MSPS")
        
        self._packets_label.setText(f"Packets: {status.packets_received}")
        self._uptime_label.setText(f"Uptime: {int(status.uptime_seconds)}s")

