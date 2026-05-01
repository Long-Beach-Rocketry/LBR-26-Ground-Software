"""
@File:   graphs_panel.py
@Brief:  Graphs are modelled here
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Charts for altitude, velocity, acceleration, and signal are created here.
    Scrolling feature for these charts is also created here.
"""

# make sure graphs can rescale if data enlengths 

from collections import deque

from PySide6.QtCharts import QChart, QChartView, QLineSeries, QValueAxis
from PySide6.QtCore import Qt, QPropertyAnimation, QEasingCurve
from PySide6.QtGui import QColor, QPainter, QPen
from PySide6.QtWidgets import (
    QHBoxLayout,
    QLabel,
    QPushButton,
    QScrollArea,
    QSizePolicy,
    QVBoxLayout,
    QWidget)

from models.models import TelemetryFrame


graph_limit = 60

# Function: Blueprint of charts
def make_chart(title, y_label, color_hex):
    series = QLineSeries()

    pen = QPen(QColor(color_hex))
    pen.setWidth(2)
    series.setPen(pen)

    chart = QChart()
    chart.addSeries(series)
    chart.setTitle(title)
    chart.setAnimationOptions(QChart.AnimationOption.NoAnimation)
    chart.legend().hide()
    chart.setBackgroundBrush(QColor("#ffffff"))
    chart.setTitleBrush(QColor("#0f172a"))

    title_font = chart.titleFont()
    title_font.setPointSize(14)
    title_font.setBold(True)
    chart.setTitleFont(title_font)

    x_axis = QValueAxis()
    x_axis.setTitleText("Time (s)")
    x_axis.setRange(0, graph_limit)
    x_axis.setLabelFormat("%d")
    x_axis.setGridLineColor(QColor("#e2e8f0"))
    x_axis.setLabelsColor(QColor("#64748b"))
    chart.addAxis(x_axis, Qt.AlignmentFlag.AlignBottom)
    series.attachAxis(x_axis)

    y_axis = QValueAxis()
    y_axis.setTitleText(y_label)
    y_axis.setLabelFormat("%.1f")
    y_axis.setGridLineColor(QColor("#e2e8f0"))
    y_axis.setLabelsColor(QColor("#64748b"))
    chart.addAxis(y_axis, Qt.AlignmentFlag.AlignLeft)
    series.attachAxis(y_axis)

    return chart, series, y_axis

class GraphsPanel(QWidget):
    def __init__(self, parent = None):
        super().__init__(parent)

        self._alt_data = deque(maxlen = graph_limit)
        self._vel_data = deque(maxlen = graph_limit)
        self._accel_data = deque(maxlen = graph_limit)
        self._rssi_data = deque(maxlen = graph_limit)

        self._alt_ymin = [float('inf')]
        self._alt_ymax = [float('-inf')]
        self._vel_ymin = [float('inf')]
        self._vel_ymax = [float('-inf')]
        self._accel_ymin = [float('inf')]
        self._accel_ymax = [float('-inf')]
        self._rssi_ymin = [float('inf')]
        self._rssi_ymax = [float('-inf')]

        self._current_index = 0
        self._chart_count = 4

        self._build_ui()
    
    # Function: Layout of charts and scrolling feature
    def _build_ui(self):
        outer = QVBoxLayout(self)
        outer.setContentsMargins(0, 0, 0, 0)
        outer.setSpacing(0)

        nav_bar = QWidget()
        nav_bar.setObjectName("NavBar")
        nav_bar.setFixedHeight(48)
        nav_layout = QHBoxLayout(nav_bar)
        nav_layout.setContentsMargins(16, 0, 16, 0)

        self._prev_btn = QPushButton("<- Prev")
        self._prev_btn.setObjectName("NavBtn")
        self._prev_btn.setFixedWidth(90)
        self._prev_btn.clicked.connect(self._go_prev)
        nav_layout.addWidget(self._prev_btn)

        nav_layout.addStretch()

        self._nav_title = QLabel("")
        self._nav_title.setObjectName("NavTitle")
        self._nav_title.setAlignment(Qt.AlignmentFlag.AlignCenter)
        nav_layout.addWidget(self._nav_title)

        nav_layout.addStretch()

        self._next_btn = QPushButton("Next ->")
        self._next_btn.setObjectName("NavBtn")
        self._next_btn.clicked.connect(self._go_next)
        nav_layout.addWidget(self._next_btn)

        outer.addWidget(nav_bar)

        self._scroll_area = QScrollArea()
        self._scroll_area.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self._scroll_area.setVerticalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        self._scroll_area.setFrameShape(QScrollArea.Shape.NoFrame)
        self._scroll_area.setWidgetResizable(False)

        self._charts_container = QWidget()
        self._charts_layout = QHBoxLayout(self._charts_container)
        self._charts_layout.setContentsMargins(0, 0, 0, 0)
        self._charts_layout.setSpacing(0)

        self._alt_chart, self._alt_series, self._alt_yaxis = make_chart("Altitude", "m", "#3b82f6")
        self._vel_chart, self._vel_series, self._vel_yaxis = make_chart("Velocity", "m/s", "#22c55e")
        self._accel_chart, self._accel_series, self._accel_yaxis = make_chart("Acceleration", "m/s", "#f97316")
        self._rssi_chart, self._rssi_series, self._rssi_yaxis = make_chart("Signal (RSSI)", "dBm", "#a855f7")

        self._charts = [
            ("Altitude", self._alt_chart, self._alt_series, self._alt_yaxis, self._alt_data),
            ("Velocity", self._vel_chart, self._vel_series, self._vel_yaxis, self._vel_data),
            ("Acceleration", self._accel_chart, self._accel_series, self._accel_yaxis, self._accel_data),
            ("Signal (RSSI)", self._rssi_chart, self._rssi_series, self._rssi_yaxis, self._rssi_data)
        ]

        self._chart_views = []
        
        for _, chart, _, _, _ in self._charts:
            view = QChartView(chart)
            view.setRenderHint(QPainter.RenderHint.Antialiasing)

            view.setSizePolicy(QSizePolicy.Policy.Fixed, QSizePolicy.Policy.Expanding)
            self._charts_layout.addWidget(view)
            self._chart_views.append(view)
        
        self._scroll_area.setWidget(self._charts_container)
        outer.addWidget(self._scroll_area)
        
        dot_bar = QWidget()
        dot_bar.setObjectName("DotBar")
        dot_bar.setFixedHeight(32)
        dot_layout = QHBoxLayout(dot_bar)
        dot_layout.setAlignment(Qt.AlignmentFlag.AlignCenter)
        dot_layout.setSpacing(8)
        self._dots = []

        for _ in self._charts:
            dot = QLabel("*")
            dot.setObjectName("Dot")
            dot_layout.addWidget(dot)
            self._dots.append(dot)
        
        outer.addWidget(dot_bar)

        self._scroll_anim = QPropertyAnimation(self._scroll_area.horizontalScrollBar(), b"value")
        self._scroll_anim.setDuration(300)
        self._scroll_anim.setEasingCurve(QEasingCurve.Type.OutCubic)

        self._apply_style()
        self._update_nav()
    
    def _apply_style(self):
        self.setStyleSheet("""
            QWidget#NavBar {
                background: #f8fafc;
                border-bottom: 1px solid #e2e8f0;
            }
            QLabel#NavTitle {
                font-size: 14px;
                font-weight: 600;
                color: #0f172a;
                font-family: 'Segoe UI', sans-serif;
            }
            QPushButton#NavBtn {
                background: transparent;
                border: 1px solid #e2e8f0;
                border-radius: 6px;
                padding: 6px 14px;
                font-size: 12px;
                color: #0f172a;
                font-weight: 500;
            }
            QPushButton#NavBtn:hover {
                background: #f1f5f9;
                border-color: #94a3b8;
            }
            QPushButton#NavBtn:disabled {
                color: #cbd5e1;
                border-color: #f1f5f9;
            }
            QWidget#DotBar {
                background: #f8fafc;
                border-top: 1px solid #e2e8f0;
            }
            QLabel#Dot {
                font-size: 16px;
                color: #cbd5e1;
            }
            QLabel#Dot[active="true"] {
                color: #3b82f6;
            }
        """)

    def _go_prev(self):
        if self._current_index > 0:
            self._current_index -= 1
            self._scroll_to_current()
    
    def _go_next(self):
        if self._current_index < self._chart_count -1:
            self._current_index += 1
            self._scroll_to_current()
    
    def _scroll_to_current(self):
        chart_width = self._scroll_area.viewport().width()
        target_x = self._current_index * chart_width

        self._scroll_anim.stop()
        self._scroll_anim.setStartValue(self._scroll_area.horizontalScrollBar().value())
        self._scroll_anim.setEndValue(target_x)
        self._scroll_anim.start()

        self._update_nav()
    
    def _update_nav(self):
        name = self._charts[self._current_index][0]
        self._nav_title.setText(f"{name} ({self._current_index + 1} / {self._chart_count})")

        self._prev_btn.setEnabled(self._current_index > 0)
        self._next_btn.setEnabled(self._current_index < self._chart_count - 1)

        for i, dot in enumerate(self._dots):
            dot.setProperty("active", "true" if i == self._current_index else "false")
            dot.style().unpolish(dot)
            dot.style().polish(dot)

    def resizeEvent(self, event):
        super().resizeEvent(event)
        self._resize_charts()
    
    def showEvent(self, event):
        super().showEvent(event)
        self._resize_charts()

    def _resize_charts(self):
        viewport = self._scroll_area.viewport()
        chart_width = viewport.width()
        chart_height = viewport.height()
        total_width = chart_width * self._chart_count

        self._charts_container.setFixedSize(total_width, chart_height)

        for view in self._chart_views:
            view.setFixedSize(chart_width, chart_height)

        self._scroll_area.horizontalScrollBar().setValue(self._current_index * chart_width)

    def _update_series(self, series, y_axis, data_deque, y_min_tracker, y_max_tracker):
        series.clear()
        for x, y in enumerate(data_deque):
            series.append(x, y)

        if data_deque:
            y_min_tracker[0] = min(y_min_tracker[0], min(data_deque))
            y_max_tracker[0] = max(y_max_tracker[0], max(data_deque))
            margin = max(abs(y_max_tracker[0] - y_min_tracker[0]) * 0.1, 1.0)
            y_axis.setRange(y_min_tracker[0] - margin, y_max_tracker[0] + margin)

    def update_frame(self, frame: TelemetryFrame):
        self._alt_data.append(frame.altitude)
        self._vel_data.append(frame.velocity)
        self._accel_data.append(frame.acceleration)
        self._rssi_data.append(frame.signal)

        self._update_series(self._alt_series, self._alt_yaxis, self._alt_data, self._alt_ymin, self._alt_ymax)
        self._update_series(self._vel_series, self._vel_yaxis, self._vel_data, self._vel_ymin, self._vel_ymax)
        self._update_series(self._accel_series, self._accel_yaxis, self._accel_data, self._accel_ymin, self._accel_ymax)
        self._update_series(self._rssi_series, self._rssi_yaxis, self._rssi_data, self._rssi_ymin, self._rssi_ymax)

        for _, chart, _, _, data in self._charts:
            self._update_series(chart.series()[0], chart.axes(Qt.Orientation.Vertical)[0], data, [float('inf')], [float('-inf')])