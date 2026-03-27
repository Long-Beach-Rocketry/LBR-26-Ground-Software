"""
@File:   main.py
@Brief:  Main file which gets everything running
@Author: Mario Cruz
@Orgin:  Long Beach Rocketry

Description:
    Main function initializes Mainwindow and MockDataSoure, beggining the GUI.
    To run type in terminal "python gui/main.py" (not sure if works for everyone)
"""


#TODO: Add a Pascal scale with units
#     Terminal that type in command (ex. command a rocket to lauch, etc. for now make a terminal that we can link to the COM port of router)
#     Add a graph that show the connection to the board basically all of the content in the table you have right now translate to ( ) vs ( ) that is make senses

import sys
from PySide6.QtWidgets import QApplication
from data.mockdata import MockDataSource
from ui.main_window import MainWindow


def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Ground Station GUI")

    # MockDataSource will send telementry data every 1 second
    # TODO: Change MockData with real data source in future
    source = MockDataSource(interval_ms = 1000)

    window = MainWindow(datasource = source)
    window.show()

    sys.exit(app.exec())

if __name__ == "__main__":
    main()