from datetime import datetime
from PySide6.QtCore import QTimer
from data.data import DataSource
from models.models import ConnectionState, SDRConfig, SystemStatus, TelemetryFrame


Mock_SDR_Config = SDRConfig(
    device = "rtlsdr",
    sample_rate_hz = 2_048_000,
    center_freq_hz = 433_920_000,
    gain_db = 30.0,
    output_path = "output/frame.bin",
    verbose = False,
)


Mock_Frames = [
    TelemetryFrame (
        altitude = 0.0,
        velocity = 0.0,
        acceleration = 0.0,
        temperature = 22.5,
        battery = 8.32,
        pressure_pa = 101325.0,
        signal = -61.0,
        packet_count = 1,
    ),

    TelemetryFrame (
        altitude = 12.4,
        velocity = 18.7,
        acceleration = 31.2,
        temperature = 21.8,
        battery = 8.30,
        pressure_pa = 100900.0,
        signal = -62.5,
        packet_count = 2,
    ),

    TelemetryFrame (
        altitude = 87.3,
        velocity = 52.1,
        acceleration = 28.4,
        temperature = 2.1,
        battery = 8.28,
        pressure_pa = 99200.0,
        signal = -65.0,
        packet_count = 3,
    ),

    TelemetryFrame (
        altitude = 214.6,
        velocity = 43.8,
        acceleration = -9.7,
        temperature = 15.1,
        battery = 8.25,
        pressure_pa = 97800.0,
        signal = -68.2,
        packet_count = 4,
    ),

    TelemetryFrame (
        altitude = 298.0,
        velocity = 12.3,
        acceleration = -9.8,
        temperature = 10.5,
        battery = 8.22,
        pressure_pa = 97500.0,
        signal = -70.1,
        packet_count = 5,
    ),
]


class MockDataSource(DataSource):
    def __init__(self, interval_ms = 1000):
        super().__init__()

        self.interval_ms = interval_ms
        self.frame_index = 0
        self.latest_frame = None

        self.status = SystemStatus(
            connection_state = ConnectionState.Disconnected,
            sdr_config = Mock_SDR_Config,
        )

        self.timer = QTimer()
        self.timer.timeout.connect(self._tick)

    def connect(self):
        self.status.connection_state = ConnectionState.Connected
        self._emit_status(self.status)
        self.timer.start(self.interval_ms)

    def disconnect(self):
        self.timer.stop()
        self.status.connection_state = ConnectionState.Disconnected
        self._emit_status(self.status)

    def get_latest_frame(self):
        return self.latest_frame
    
    def get_status(self):
        return self.status
    
    def _tick(self):
        template = Mock_Frames[self.frame_index % len(Mock_Frames)]

        frame = TelemetryFrame(
            timestamp = datetime.now(),
            altitude = template.altitude,
            velocity = template.velocity,
            acceleration = template.acceleration,
            temperature = template.temperature,
            battery = template.battery,
            pressure_pa = template.pressure_pa,
            signal = template.signal,
            packet_count = template.packet_count,
        )

        self.latest_frame = frame
        self.frame_index += 1
        
        self.status.packets_received += 1
        self.status.last_packet_time = frame.timestamp
        self.status.uptime_seconds += self.interval_ms / 1000

        self._emit_frame(frame)
        self._emit_status(self.status)