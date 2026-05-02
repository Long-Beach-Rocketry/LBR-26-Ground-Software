#!/usr/bin/env python3
"""
@File:   main.py
@Brief:  Main file which gets everything running
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Main function initializes Mainwindow and MockDataSoure, beggining the GUI.
    To run type in terminal "python gui/main.py" (not sure if works for everyone)
"""


import argparse
import sys
from PySide6.QtWidgets import QApplication
from data.factory import create_datasource
from ui.main_window import MainWindow


def _parse_args(argv=None):
    parser = argparse.ArgumentParser(description="Ground Station GUI")
    parser.add_argument(
        "--source",
        default="mock",
        choices=["mock"],
        help="Data source backend to use",
    )
    parser.add_argument(
        "--interval-ms",
        type=int,
        default=1000,
        help="Update interval in milliseconds for mock source",
    )
    return parser.parse_args(argv)


def main():
    args = _parse_args()

    app = QApplication(sys.argv)
    app.setApplicationName("Ground Station GUI")

    source = create_datasource(mode=args.source, interval_ms=args.interval_ms)

    window = MainWindow(datasource = source)
    window.show()

    sys.exit(app.exec())

if __name__ == "__main__":
    main()