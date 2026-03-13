from datetime import datetime
from enum import Enum, auto

from PySide6.QtGui import QColor, QTextCharFormat, QTextCursor
from PySide6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QPushButton,
    QSizePolicy,
    QTextEdit,
    QVBoxLayout,
    QWidget,
)

class LogLevel(Enum):
    Info = auto()
    Warn = auto()
    Error = auto()
    Debug = auto()

Level_Colors = {
    LogLevel.Info:  "#0f172a",
    LogLevel.Warn:  "#b45309",
    LogLevel.Error: "#dc2626",
    LogLevel.Debug: "#6366f1",
}


Level_Prefix = {
    LogLevel.Info: "Info",
    LogLevel.Warn: "Warn",
    LogLevel.Error: "Error",
    LogLevel.Debug: "Debug",
}

class LogPanel(QWidget):
    Max_Lines = 500

    def __init__(self, parent = None):
        super().__init__(parent)
        self._line_count = 0
        self._build_ui()

    def _build_ui(self):
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0, 0, 0, 0)
        layout.setSpacing(0)

        header = QWidget()
        header.setObjectName("LogHeader")
        header.setFixedHeight(36)
        header_layout = QHBoxLayout(header)
        header_layout.setContentsMargins(12, 0, 12, 0)

        title = QLabel("Event Log")
        title.setStyleSheet(
            "font-size: 10px; font-weight: 700; letter-spacing: 2px;"
            "color: #64748b; font-family: 'Courier New', monospace;"
        )
        header_layout.addWidget(title)
        header_layout.addStretch()

        self._clear_btn = QPushButton("Clear")
        self._clear_btn.setObjectName("ClearBtn")
        self._clear_btn.clicked.connect(self.clear)
        header_layout.addWidget(self._clear_btn)

        layout.addWidget(header)

        self._text = QTextEdit()
        self._text.setReadOnly(True)
        self._text.setObjectName("LogText")
        self._text.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
        layout.addWidget(self._text)

        self._apply_style()

    def _apply_style(self):
        self.setStyleSheet("""
            QWidget#LogHeader {
                background: #f8fafc;
                border-top: 1px solid #e2e8f0;
                border-bottom: 1px solid #e2e8f0;
            }
            QTextEdit#LogText {
                background: #ffffff;
                border: none;
                font-family: 'Courier New', monospace;
                font-size: 11px;
                color: #0f172a;
                padding: 8px 12px;
            }
            QPushButton#ClearBtn {
                background: transparent;
                border: 1px solid #e2e8f0;
                border-radius: 4px;
                padding: 2px 10px;
                font-size: 11px;
                color: #64748b;
                font-family: 'Courier New', monospace;
            }
            QPushButton#ClearBtn:hover {
                background: #f1f5f9;
                border-color: #94a3b8;
            }
        """)

    def log(self, message, level = LogLevel.Info):
        if self._line_count >= self.Max_Lines:
            cursor = self._text.textCursor()
            cursor.movePosition(QTextCursor.MoveOperation.Start)
            cursor.select(QTextCursor.SelectionType.LineUnderCursor)
            cursor.removeSelectedText()
            cursor.deleteChar()
            self._line_count -= 1

        timestamp = datetime.now().strftime("%H:%M:%S.%f")[:-3]
        prefix = Level_Prefix[level]
        color = Level_Colors[level]

        cursor = self._text.textCursor()
        cursor.movePosition(QTextCursor.MoveOperation.End)

        ts_format = QTextCharFormat()
        ts_format.setForeground(QColor("#94a3b8"))
        cursor.insertText(f"{timestamp} ", ts_format)

        level_format = QTextCharFormat()
        level_format.setForeground(QColor(color))
        level_format.setFontWeight(700)
        cursor.insertText(f"{prefix} ", level_format)

        msg_format = QTextCharFormat()
        msg_format.setForeground(QColor(color))
        cursor.insertText(f"{message}\n", msg_format)

        self._line_count +=1 

        self._text.setTextCursor(cursor)
        self._text.ensureCursorVisible()

    def log_info(self, message): self.log(message, LogLevel.Info)
    def log_warn(self, message): self.log(message, LogLevel.Warn)
    def log_error(self, message): self.log(message, LogLevel.Error)
    def log_debug(self, message): self.log(message, LogLevel.Debug)

    def clear(self):
        self._text.clear()
        self._line_count = 0