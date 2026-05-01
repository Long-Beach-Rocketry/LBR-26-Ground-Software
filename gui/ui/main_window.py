"""
@File:   main_window.py
@Brief:  Main Window of the GUI
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    MainWindow is the core of the GUI, contains all widgets and handles
    data flow. MainWindow receives telemetry frames and connection statsus updates.
"""

from PySide6.QtCore import Qt
from PySide6.QtWidgets import (
    QMainWindow,
    QScrollArea,
    QSizePolicy,
    QSplitter,
    QTabWidget,
    QVBoxLayout,
    QWidget,
)

from data.data import DataSource
from models.models import SystemStatus, TelemetryFrame
from ui.widgets.log_panel import LogPanel
from ui.widgets.status_bar import StatusBar
from ui.widgets.telemetry_cards import TelemetryCardsPanel
from ui.widgets.graphs_panel import GraphsPanel
from ui.widgets.terminal_panel import TerminalPanel
from ui.widgets.packet_viewer import PacketViewerPanel

class MainWindow(QMainWindow):
    def __init__(self, datasource: DataSource):
        super().__init__()

        self._source = datasource

        # Window setup
        self.setWindowTitle("Ground Station GUI")
        self.setMinimumSize(960, 680)

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
        self._log.log_info("Open termianl tab and connect port to send commands")

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

        self._tabs = QTabWidget()
        self._tabs.setObjectName("MainTabs")

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
        self._tabs.addTab(scroll_area, "📊  Telemetry")
 
        self._graphs = GraphsPanel()
        self._tabs.addTab(self._graphs, "📈  Graphs")

        self._terminal = TerminalPanel()
        self._tabs.addTab(self._terminal, "💻  Terminal")
 
        self._packet_viewer = PacketViewerPanel()
        self._tabs.addTab(self._packet_viewer, "📦  Packets")

        splitter.addWidget(self._tabs)

        self._log = LogPanel()
        splitter.addWidget(self._log)

        splitter.setSizes([500, 180])

        root_layout.addWidget(splitter)

    def _apply_style(self):
        self.setStyleSheet("""
            QMainWindow, QWidget {
                background-color: #f8fafc;
                font-family: 'Segoe UI', 'SF Pro Text', 'Helvetica Neue', sans-serif;
            }
            QTabWidget#MainTabs::pane {
                border: none;
                background: #f8fafc;
            }
            QTabWidget#MainTabs > QTabBar::tab {
                background: #f1f5f9;
                color: #64748b;
                padding: 8px 20px;
                border: none;
                border-bottom: 2px solid transparent;
                font-size: 12px;
                font-weight: 500;
                min-width: 120px;
            }
            QTabWidget#MainTabs > QTabBar::tab:selected {
                background: #ffffff;
                color: #0f172a;
                border-bottom: 2px solid #3b82f6;
                font-weight: 600;
            }
            QTabWidget#MainTabs > QTabBar::tab:hover {
                background: #e2e8f0;
                color: #0f172a;
            }
            QSplitter::handle {
                background: #e2e8f0;
            }
            QScrollArea {
                background: transparent;
            }
            QScrollBar:vertical {
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
        self._graphs.update_frame(frame)

        self._log.log_debug(
            f"pkt {frame.packet_count:04d}"
            f"alt = {frame.altitude:.1f}m"
            f"vel = {frame.velocity:.1f}m/s"
            f"pres = {frame.pressure_pa:.0f}Pa"
            f"rssi = {frame.signal:.1f}dBm"
        )
    
    def _on_status_change(self, status: SystemStatus):
        self._status_bar.update_status(status)

    def closeEvent(self, event):
        self._source.disconnect()
        self._terminal.closeEvent(event)
        self._log.log_info("Disconnecting - goodbye")
        super().closeEvent(event)