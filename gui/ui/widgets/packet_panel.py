from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QGroupBox, QFormLayout, QLabel, QTextEdit
)

from .packet_panel_formatting import format_packet_view


class PacketViewerPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        layout = QVBoxLayout(self)

        self._time_box = QGroupBox("Time")
        self._time_form = QFormLayout(self._time_box)
        self._packet_id = QLabel("-")
        self._unix_ms = QLabel("-")
        self._iso_time = QLabel("-")
        self._time_form.addRow("Packet ID:", self._packet_id)
        self._time_form.addRow("Unix ms:", self._unix_ms)
        self._time_form.addRow("ISO Time:", self._iso_time)

        self._data_box = QGroupBox("Data")
        self._data_form = QFormLayout(self._data_box)
        self._altitude = QLabel("-")
        self._velocity = QLabel("-")
        self._accel = QLabel("-")
        self._temp = QLabel("-")
        self._battery = QLabel("-")
        self._pressure = QLabel("-")
        self._signal = QLabel("-")
        self._pkt_count = QLabel("-")
        self._data_form.addRow("Altitude:", self._altitude)
        self._data_form.addRow("Velocity:", self._velocity)
        self._data_form.addRow("Acceleration:", self._accel)
        self._data_form.addRow("Temperature:", self._temp)
        self._data_form.addRow("Battery:", self._battery)
        self._data_form.addRow("Pressure:", self._pressure)
        self._data_form.addRow("Signal:", self._signal)
        self._data_form.addRow("Packet Count:", self._pkt_count)

        self._status_box = QGroupBox("Status")
        self._status_form = QFormLayout(self._status_box)
        self._conn_state = QLabel("-")
        self._device = QLabel("-")
        self._pkts_rx = QLabel("-")
        self._pkts_lost = QLabel("-")
        self._uptime = QLabel("-")
        self._status_text = QLabel("-")
        self._status_form.addRow("Connection:", self._conn_state)
        self._status_form.addRow("Device:", self._device)
        self._status_form.addRow("Packets RX:", self._pkts_rx)
        self._status_form.addRow("Packets Lost:", self._pkts_lost)
        self._status_form.addRow("Uptime:", self._uptime)
        self._status_form.addRow("Text:", self._status_text)

        self._messages_box = QGroupBox("Messages")
        self._messages_text = QTextEdit()
        self._messages_text.setReadOnly(True)

        layout.addWidget(self._time_box)
        layout.addWidget(self._data_box)
        layout.addWidget(self._status_box)
        layout.addWidget(self._messages_box)
        layout.addWidget(self._messages_text)

    def update_packet(self, packet):
        values = format_packet_view(packet)

        self._packet_id.setText(values["packet_id"])
        self._unix_ms.setText(values["unix_ms"])
        self._iso_time.setText(values["iso_time"])

        self._altitude.setText(values["altitude"])
        self._velocity.setText(values["velocity"])
        self._accel.setText(values["accel"])
        self._temp.setText(values["temp"])
        self._battery.setText(values["battery"])
        self._pressure.setText(values["pressure"])
        self._signal.setText(values["signal"])
        self._pkt_count.setText(values["pkt_count"])

        self._conn_state.setText(values["conn_state"])
        self._device.setText(values["device"])
        self._pkts_rx.setText(values["pkts_rx"])
        self._pkts_lost.setText(values["pkts_lost"])
        self._uptime.setText(values["uptime"])
        self._status_text.setText(values["status_text"])

        self._messages_text.setPlainText(values["messages"])