from collections import deque

from PySide6.QtCharts import QChart, QChartView, QLineSeries, QValueAxis
from PySide6.QtCore import Qt
from PySide6.QtGui import QColor, QPainter, QPen
from PySide6.QtWidgets import QGridLayout, QLabel, QSizePolicy, QVBoxLayout, QWidget

from models.models import TelemetryFrame

max_points = 60

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

    x_axis = QValueAxis()
    x_axis.setTitleText("Time (s)")
    x_axis.setRange(0, max_points)
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

        self._alt_data = deque(maxlen = max_points)
        self._vel_data = deque(maxlen = max_points)
        self._accel_data = deque(maxlen = max_points)
        self._rssi_data = deque(maxlen = max_points)

        self._tick = 0

        self._build_ui()
    
    def _build_ui(self):
        layout = QGridLayout(self)
        layout.setSpacing(12)
        layout.setContentsMargins(16, 16, 16, 16)

        self._alt_chart, self._alt_series, self._alt_yaxis = make_chart("Altitude", "m", "#3b82f6")
        self._vel_chart, self._vel_series, self._vel_yaxis = make_chart("Velocity", "m/s", "#22c55e")
        self._accel_chart, self._accel_series, self._accel_yaxis = make_chart("Acceleration", "m/s", "#f97316")
        self._rssi_chart, self._rssi_series, self._rssi_yaxis = make_chart("Signal (RSSI)", "dBm", "#a855f7")

        def make_view(chart):
            view = QChartView(chart)
            view.setRenderHint(QPainter.RenderHint.Antialiasing)
            view.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Expanding)
            return view
        
        layout.addWidget(make_view(self._alt_chart), 0, 0)
        layout.addWidget(make_view(self._vel_chart), 0, 1)
        layout.addWidget(make_view(self._accel_chart), 1, 0)
        layout.addWidget(make_view(self._rssi_chart), 1, 1)

    def _update_series(self, series, y_axis, data_deque):
        points = [(i, v) for i, v in enumerate(data_deque)]

        series.replace([
            series.at(0).__class__.__new__(series.at(0).__class__)
            for _ in points
        ] if False else [])

        series.clear()

        for x, y in points:
            series.append(x, y)

        if data_deque:
            min_val = min(data_deque)
            max_val = max(data_deque)
            margin = max(abs(max_val - min_val) * 0.1, 1.0)
            y_axis.setRange(min_val - margin, max_val + margin)

    def update_frame(self, frame: TelemetryFrame):
        self._alt_data.append(frame.altitude)
        self._vel_data.append(frame.velocity)
        self._accel_data.append(frame.acceleration)
        self._rssi_data.append(frame.signal)
 
        self._tick += 1
 
        self._update_series(self._alt_series,   self._alt_yaxis,   self._alt_data)
        self._update_series(self._vel_series,   self._vel_yaxis,   self._vel_data)
        self._update_series(self._accel_series, self._accel_yaxis, self._accel_data)
        self._update_series(self._rssi_series,  self._rssi_yaxis,  self._rssi_data)
 