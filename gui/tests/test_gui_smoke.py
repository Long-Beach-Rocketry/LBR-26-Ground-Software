import os
import stat
from pathlib import Path

os.environ.setdefault("QT_QPA_PLATFORM", "offscreen")

from PySide6.QtWidgets import QApplication

from data.mockdata import MockDataSource
from models.models import ConnectionState
from ui.main_window import MainWindow


REPO_ROOT = Path(__file__).resolve().parents[2]
GUI_MAIN = REPO_ROOT / "gui" / "main.py"


def _qapp():
    app = QApplication.instance()
    if app is None:
        app = QApplication([])
    return app


def test_main_entrypoint_has_linux_shebang():
    assert GUI_MAIN.read_text(encoding="utf-8").splitlines()[0] == "#!/usr/bin/env python3"

    if os.name == "posix":
        assert GUI_MAIN.stat().st_mode & stat.S_IXUSR


def test_mock_data_source_emits_frames():
    _qapp()
    source = MockDataSource(interval_ms=1000)
    frames = []
    states = []

    source.on_frame(frames.append)
    source.on_status_change(lambda status: states.append(status.connection_state))
    source.connect()
    source._tick()
    source.disconnect()

    assert source.get_latest_frame() is frames[-1]
    assert frames[-1].packet_count == 1
    assert states[0] == ConnectionState.Connected
    assert states[-1] == ConnectionState.Disconnected


def test_main_window_constructs_with_mock_data_source():
    _qapp()
    source = MockDataSource(interval_ms=1000)
    window = MainWindow(datasource=source)

    try:
        assert window.windowTitle() == "Ground Station GUI"
        assert source.get_status().connection_state == ConnectionState.Connected
    finally:
        window.close()
