"""
@File:   data.py
@Brief:  Abstract class, blueprint all data sources must follow 
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Data sources (mock, radio frequency, serial port, etc) must be able to:
    - Connect
    - Disconnect
    - Return latest telelemtry frame
    - Return current connection status
"""

from abc import ABC, abstractmethod
from typing import Optional
from models.models import TelemetryFrame, SystemStatus


class DataSource(ABC):
    def __init__(self):
        self._frame_callback = None  # telementry frame (packet of data [i think])
        self._status_callback = None # connection status

    """ Main Window calls these functions """
    # Function: Called when new telelemtry frame arrives
    def on_frame(self, callback):
        self._frame_callback = callback

    # Function: Called when connection status changes
    def on_status_change(self, callback):
        self._status_callback = callback

    def on_packet(self, callback):
        self._packet_callback = callback

    """ Abstract methods (completed in subclasses [mock / read data]) """
    # Function: Starts the data source (connection, status, timer)
    @abstractmethod
    def connect(self):
        pass

    # Function: Stops timer, connection, updates status
    @abstractmethod
    def disconnect(self):
        pass

    # Function: Returns most recent telemetry frame or None
    @abstractmethod
    def get_latest_frame(self) -> Optional[TelemetryFrame]:
        pass

    # Function: Returns current connection status
    @abstractmethod
    def get_status(self) -> SystemStatus:
        pass

    """ Internal helpers used by subclasses """
    # Function: Sends new frame if exists
    def _emit_frame(self, frame: TelemetryFrame):
        if self._frame_callback is not None:
            self._frame_callback(frame)

    #Function: Sends connection status update if exists
    def _emit_status(self, status: SystemStatus):
        if self._status_callback is not None:
            self._status_callback(status)

    def _emit_packet(self, packet: bytes):
        if self._packet_callback is not None:
            self._packet_callback(packet)