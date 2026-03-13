from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QMainWindow,
    QWidget,
    QVBoxLayout,
    QScrollArea,
    QSplitter,
    QSizePolicy,
)

from ground_gui.data.data import DataSource
from ground_gui.models.models import SystemStatus, TelemetryFrame
from ground_gui.ui.widgets.log_panel import LogPanel
from ground_gui.ui.widgets.status_bar import StatusBar
from ground_gui.ui.widgets.telemetry_cards import TelemetryCardsPanel

class MainWindow(QMainWindow):
    def __init__(self, datasource: DataSource):
        super().__init__()

        self._source = datasource

        # Window setup
        self.setWindowTitle("Ground Station GUI")
        self.setMinimumSize(860, 600)

        # Building the UI
        self._build_ui()
        self._apply_style()
        self._connect_datasource()

        # Start receiving data
        self._source.connect()

        # Logging startup information
        self._log.log_info("Ground station started - currently running mock data")
        cfg = self._source.get_status().sdr_config
        self._log.log_info(
            f"SDR config: {cfg.device.upper()} at {cfg.center_freq_mhz:.1f} MHz"
            f"SR: {cfg.sample_rate_mhz:.1f} MSPS Gain: {cfg.gain_db} dB"
        )

    def _build_ui(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        root_layout = QVBoxLayout(central_widget)
        root_layout.setContentsMargins(0, 0, 0, 0)
        root_layout.setSpacing(0)

        self._status_bar = StatusBar()
        root_layout.addWidget(self._status_bar)

        splitter = QSplitter(Qt.Orientation.Vertical)
        splitter.setHandleWidth(4)

        scroll_area = QScrollArea()
        scroll_area.setWidgetResizable(True)
        scroll_area.setFrameShape(QScrollArea.Shape.NoFrame)
        scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)

        cards_container = QWidget()
        cards_layout = QVBoxLayout(cards_container)
        cards_layout.setContentsMargins(20, 16, 20, 16)

        self._cards = TelemetryCardsPanel()
        cards_layout.addWidget(self._cards)
        cards_layout.addStretch()

        scroll_area.setWidget(cards_container)
        splitter.addWidget(scroll_area)

        self._log = LogPanel()
        splitter.addWidget(self._log)

        splitter.setSizes([420, 180])

        root_layout.addWidget(splitter)

    def _apply_style(self):
        self.setStyleSheet("""
            QMainWindow, QWidget {
                background-color: #f8fafc;
                front-family: 'Segoe UI', 'SF Pro Text', 'Helvetica Neue', sans-serif;
            }
            QSplitter::handle {
                background: #e2e8f0;
            }
            QScrollArea {
                background: transparent;
            }
            QScrollBar::vertical {
                background: #f1f5f9;
                width: 8px;
                border-radius: 4px;    
            }
            QScrollBar::handle:vertical {
                background: #cbd5e1;
                border-radius: 4px;
                min-height: 20px;
            }
            QScrollBar::handle:vertical:hover {
                background: #94a3b8;               
            }
            QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
                height: 0;            
            }
        """)
    
    def _connect_datasource(self):
        self._source.on_frame(self._on_new_frame)
        self._source.on_status_change(self._on_status_change)

    def _on_new_frame(self, frame: TelemetryFrame):
        self._cards.update_frame(frame)

        self._log.log_debug(
            f"pkt {frame.packet_count:04d}"
            f"alt = {frame.altitude:.1f}m"
            f"vel = {frame.velocity:.1f}m/s"
            f"rssi = {frame.signal:.1f}dBm"
        )
    
    def _on_status_change(self, status: SystemStatus):
        self._status_bar.update_status(status)

    def closeEvent(self, event):
        self._source.disconnect()
        self._log.log_info("Disconnecting - goodbye")
        super().closeEvent(event)