from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QGroupBox, QFormLayout, QLabel, QTextEdit
)

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
        self._packet_id.setText(str(packet.packet_id))
        self._unix_ms.setText(str(packet.time.unix_ms))
        self._iso_time.setText(packet.time.iso_time)

        self._altitude.setText(f"{packet.data.altitude_m:.2f} m")
        self._velocity.setText(f"{packet.data.velocity_mps:.2f} m/s")
        self._accel.setText(f"{packet.data.acceleration_mps2:.2f} m/s²")
        self._temp.setText(f"{packet.data.temperature_c:.2f} C")
        self._battery.setText(f"{packet.data.battery_v:.2f} V")
        self._pressure.setText(f"{packet.data.pressure_pa:.2f} Pa")
        self._signal.setText(f"{packet.data.signal_dbm:.2f} dBm")
        self._pkt_count.setText(str(packet.data.packet_count))

        self._conn_state.setText(str(packet.status.connection_state))
        self._device.setText(packet.status.device)
        self._pkts_rx.setText(str(packet.status.packets_received))
        self._pkts_lost.setText(str(packet.status.packets_lost))
        self._uptime.setText(f"{packet.status.uptime_seconds:.1f} s")
        self._status_text.setText(packet.status.status_text)

        lines = []
        for msg in packet.messages:
            lines.append(f"[{msg.level}] {msg.text}")
        self._messages_text.setPlainText("\n".join(lines))