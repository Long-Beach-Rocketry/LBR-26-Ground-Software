"""
@File:   terminal_panel.py
@Brief:  a
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    a
"""

import serial
import serial.tools.list_ports
from datetime import datetime

from PySide6.QtCore import Qt, QThread, Signal, QObject
from PySide6.QtGui import QColor, QTextCharFormat, QTextCursor
from PySide6.QtWidgets import (
    QComboBox,
    QHBoxLayout,
    QLabel,
    QLineEdit,
    QPushButton,
    QSizePolicy,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

class SerialReader(QObject):
    data_received = Signal(str)
    error_ocurred = Signal(str)

    def __init__(self, port):
        super().__init__()
        self._port = port
        self._running = False

    def start_reading(self):
        self._running = True
        while self._running:
            try:
                raw = self._port.readline()
                if raw:
                    text = raw.decode("utf-8", errors="replace").strip()
                    if text:
                        self.data_received.emit(text)
            except Exception as e:
                if self._running:
                    self.error_occurred.emit(str(e))
                break
    
    def stop(self):
        self._running = False

class TerminalPanel(QWidget):
    baud_rate = 115200

    def __init__(self, parent = None):
        super().__init__(parent)

        self._serial_port = None
        self._reader = None
        self._reader_thread = None

        self._build_ui()
        self._refresh_ports()

    def _build_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(16, 16, 16, 16)
        layout.setSpacing(8)

        top_bar = QHBoxLayout()
 
        port_label = QLabel("Port:")
        port_label.setFixedWidth(36)
        top_bar.addWidget(port_label)
 
        self._port_combo = QComboBox()
        self._port_combo.setMinimumWidth(180)
        self._port_combo.setToolTip("Select the COM port your router is connected to")
        top_bar.addWidget(self._port_combo)
 
        self._refresh_btn = QPushButton("↺ Refresh")
        self._refresh_btn.setObjectName("RefreshBtn")
        self._refresh_btn.clicked.connect(self._refresh_ports)
        self._refresh_btn.setFixedWidth(90)
        top_bar.addWidget(self._refresh_btn)
 
        top_bar.addSpacing(12)
 
        baud_label = QLabel("Baud:")
        baud_label.setFixedWidth(36)
        top_bar.addWidget(baud_label)
 
        self._baud_combo = QComboBox()
        for rate in ["9600", "19200", "38400", "57600", "115200", "230400"]:
            self._baud_combo.addItem(rate)
        self._baud_combo.setCurrentText("115200")
        self._baud_combo.setFixedWidth(90)
        top_bar.addWidget(self._baud_combo)
 
        top_bar.addStretch()
 
        self._connect_btn = QPushButton("Connect")
        self._connect_btn.setObjectName("ConnectBtn")
        self._connect_btn.setFixedWidth(100)
        self._connect_btn.clicked.connect(self._toggle_connection)
        top_bar.addWidget(self._connect_btn)
 
        layout.addLayout(top_bar)
 
        self._output = QTextEdit()
        self._output.setReadOnly(True)
        self._output.setObjectName("TerminalOutput")
        self._output.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        layout.addWidget(self._output)
 
        bottom_bar = QHBoxLayout()
 
        self._input = QLineEdit()
        self._input.setObjectName("TerminalInput")
        self._input.setPlaceholderText("Type a command and press Enter...")
        self._input.setEnabled(False)
        self._input.returnPressed.connect(self._send_command)
        bottom_bar.addWidget(self._input)
 
        self._send_btn = QPushButton("Send")
        self._send_btn.setObjectName("SendBtn")
        self._send_btn.setFixedWidth(80)
        self._send_btn.setEnabled(False)
        self._send_btn.clicked.connect(self._send_command)
        bottom_bar.addWidget(self._send_btn)
 
        layout.addLayout(bottom_bar)
 
        self._apply_style()

    def _apply_style(self):
        self.setStyleSheet("""
            QTextEdit#TerminalOutput {
                background: #0f172a;
                color: #e2e8f0;
                font-family: 'Courier New', monospace;
                font-size: 12px;
                border: 1px solid #334155;
                border-radius: 6px;
                padding: 8px;
            }
            QLineEdit#TerminalInput {
                background: #1e293b;
                color: #e2e8f0;
                font-family: 'Courier New', monospace;
                font-size: 12px;
                border: 1px solid #334155;
                border-radius: 6px;
                padding: 6px 10px;
            }
            QLineEdit#TerminalInput:focus {
                border-color: #3b82f6;
            }
            QPushButton#ConnectBtn {
                background: #22c55e;
                color: white;
                font-weight: bold;
                border: none;
                border-radius: 6px;
                padding: 6px 12px;
            }
            QPushButton#ConnectBtn:hover {
                background: #16a34a;
            }
            QPushButton#ConnectBtn[connected="true"] {
                background: #ef4444;
            }
            QPushButton#SendBtn {
                background: #3b82f6;
                color: white;
                font-weight: bold;
                border: none;
                border-radius: 6px;
                padding: 6px 12px;
            }
            QPushButton#SendBtn:hover {
                background: #2563eb;
            }
            QPushButton#SendBtn:disabled, QLineEdit#TerminalInput:disabled {
                opacity: 0.4;
            }
            QPushButton#RefreshBtn {
                background: transparent;
                border: 1px solid #e2e8f0;
                border-radius: 4px;
                padding: 4px 8px;
                font-size: 11px;
                color: #64748b;
            }
            QPushButton#RefreshBtn:hover {
                background: #f1f5f9;
            }
            QComboBox {
                border: 1px solid #e2e8f0;
                border-radius: 4px;
                padding: 4px 8px;
                font-size: 11px;
                color: #0f172a;
                background: white;
            }
            QLabel {
                font-size: 11px;
                color: #64748b;
                font-family: 'Courier New', monospace;
            }
        """)
    
    def _refresh_ports(self):
        self._port_combo.clear()

        ports = serial.tools.list_ports.comports()

        if ports:
            for port in ports:
                self._port_combo.addItem(f"{port.device} — {port.description}", port.device)
        else:
            self._port_combo.addItem("No ports found")
            self._log_system("No serial ports detected. Plug in your device and click Refresh.")
    
    def _toggle_connection(self):
        if self._serial_port is None:
            self._connect()
        else:
            self._disconnect()
    
    def _connect(self):
        port_name = self._port_combo.currentData()
 
        if port_name is None:
            self._log_system("No port selected.", error=True)
            return
 
        baud = int(self._baud_combo.currentText())
 
        try:
            self._serial_port = serial.Serial(port_name, baudrate=baud, timeout=0.1)
            self._log_system(f"Connected to {port_name} at {baud} baud.")
 
            self._connect_btn.setText("Disconnect")
            self._connect_btn.setProperty("connected", "true")
            self._connect_btn.style().unpolish(self._connect_btn)
            self._connect_btn.style().polish(self._connect_btn)
            self._input.setEnabled(True)
            self._send_btn.setEnabled(True)
            self._port_combo.setEnabled(False)
            self._baud_combo.setEnabled(False)
 
            self._reader = SerialReader(self._serial_port)
            self._reader_thread = QThread()
            self._reader.moveToThread(self._reader_thread)
 
            self._reader_thread.started.connect(self._reader.start_reading)
            self._reader.data_received.connect(self._on_data_received)
            self._reader.error_occurred.connect(self._on_serial_error)
 
            self._reader_thread.start()
 
        except serial.SerialException as e:
            self._log_system(f"Failed to connect: {e}", error=True)
            self._serial_port = None
    
    def _disconnect(self):
        if self._reader:
            self._reader.stop()
 
        if self._reader_thread and self._reader_thread.isRunning():
            self._reader_thread.quit()
            self._reader_thread.wait(2000)
 
        if self._serial_port and self._serial_port.is_open:
            self._serial_port.close()
 
        self._serial_port   = None
        self._reader        = None
        self._reader_thread = None
 
        self._log_system("Disconnected.")
 
        self._connect_btn.setText("Connect")
        self._connect_btn.setProperty("connected", "false")
        self._connect_btn.style().unpolish(self._connect_btn)
        self._connect_btn.style().polish(self._connect_btn)
        self._input.setEnabled(False)
        self._send_btn.setEnabled(False)
        self._port_combo.setEnabled(True)
        self._baud_combo.setEnabled(True)
    
    def _send_command(self):
        command = self._input.text().strip()
        if not command:
            return
 
        if self._serial_port and self._serial_port.is_open:
            try:
                self._serial_port.write((command + "\n").encode("utf-8"))
                self._log_sent(command)
                self._input.clear()
            except serial.SerialException as e:
                self._log_system(f"Send failed: {e}", error=True)

    def _on_data_received(self, text):
        self._log_received(text)

    def _on_serial_error(self, error_msg):
        self._log_system(f"Serial error: {error_msg}", error=True)
        self._disconnect()

    def _log_sent(self, text):
        ts = datetime.now().strftime("%H:%M:%S")
        self._append_colored(f"[{ts}] >> {text}", "#60a5fa")
    
    def _log_received(self, text):
        ts = datetime.now().strftime("%H:%M:%S")
        self._append_colored(f"[{ts}] << {text}", "#4ade80")

    def _log_system(self, text, error = False):
        ts = datetime.now().strftime("%H:%M:%S")
        color = "#ef4444" if error else "#94a3b8"
        self._append_colored(f"[{ts}]    {text}", color)

    def _append_colored(self, text, color_hex):
        cursor = self._output.textCursor()
        cursor.movePosition(QTextCursor.MoveOperation.End)
 
        fmt = QTextCharFormat()
        fmt.setForeground(QColor(color_hex))
        cursor.insertText(text + "\n", fmt)
 
        self._output.setTextCursor(cursor)
        self._output.ensureCursorVisible()
    
    def closeEvent(self, event):
        self._disconnect()
        super().closeEvent(event)