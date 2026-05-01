"""
@File:   models.py
@Brief:  Data variables are established here
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Data types, variables, and classes are established here.
    Sort of placeholders until real or mock data can replace them in. 
"""

from dataclasses import dataclass
from datetime import datetime
from enum import Enum, auto
from typing import Optional


""" Connection State """
# Variables tied to ConnectionState class
class ConnectionState(Enum):
    Disconnected = auto()
    Connecting = auto()
    Connected = auto()
    Error = auto()

""" SDR Configuration """
# Variables tied to SDRConfig class
@dataclass
class SDRConfig:
    device: str = "rtlsdr"                # SDR device being used
    sample_rate_hz: int =  2_048_000      # Samples per second (2.048 MSPS [Mega Samples Per Second])
    center_freq_hz: int =  433_920_000    # Center frequency (433.92 MHz)
    gain_db: float =  30.0                # Decibels recevied 
    output_path: str = "output/frame.bin" # Where pipeline saves raw data
    verbose: bool = False                 # Whether to print eztra debug info

    @property
    # Helper property, converts MPSP to MHz for display
    def sample_rate_mhz(self):
        return self.sample_rate_hz / 1_000_000
    
    @property
    #Helper property, converts Hz to MHz for displays
    def center_freq_mhz(self):
        return self.center_freq_hz / 1_000_000
    
""" Telelemtry Frame """
# Constant values tied to TelelmtryFrame class
@dataclass
class TelemetryFrame:
    timestamp: datetime = None

    altitude: float = 0.0
    velocity: float = 0.0
    acceleration: float = 0.0

    temperature: float = 0.0
    battery: float = 0.0
    pressure_pa: float = 1011325.0

    signal: float = 0.0
    packet_count: int = 0

    def __post_init__(self):
        if self.timestamp is None:
            self.timestamp = datetime.now()

""" System Status """
# Constant values tied to SystemStatus class
@dataclass
class SystemStatus:
    connection_state: ConnectionState = ConnectionState.Disconnected
    sdr_config: Optional[SDRConfig] = None
    last_packet_time: Optional[datetime] = None
    packets_received: int = 0
    packets_lost: int = 0
    uptime_seconds: float = 0.0

    @property
    def is_connected(self):
        return self.connection_state == ConnectionState.Connected