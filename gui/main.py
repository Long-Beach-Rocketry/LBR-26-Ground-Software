"""

Main file which gets everything running

"""


#TODO: Add a Pascal scale with units
#     Terminal that type in command (ex. command a rocket to lauch, etc. for now make a terminal that we can link to the COM port of router)
#     Add a graph that show the connection to the board basically all of the content in the table you have right now translate to ( ) vs ( ) that is make senses

import sys
from PySide6.QtWidgets import QApplication
from gui.data.mockdata import MockDataSource
from gui.ui.main_window import MainWindow

def main():
    app = QApplication(sys.argv)
    app.setApplicationName("Ground Station GUI")

    # MockDataSource sends fake telemtry data every 1000 ms (1 second)
    # Replaced later with real data source
    source = MockDataSource(interval_ms = 1000)

    window = MainWindow(datasource = source)
    window.show()

    sys.exit(app.exec())

if __name__ == "__main__":
    main()
