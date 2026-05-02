"""
@File:   test_data_source_factory.py
@Brief:  Unit tests for data source factory
@Author: Luis Fernandes
@Orgin:  Long Beach Rocketry
"""

import sys
import unittest
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

GUI_ROOT = ROOT / "gui"
if str(GUI_ROOT) not in sys.path:
    sys.path.insert(0, str(GUI_ROOT))

from gui.data.factory import create_datasource
from data.mockdata import MockDataSource


class DataSourceFactoryTests(unittest.TestCase):
    def test_create_mock_datasource(self):
        source = create_datasource(mode="mock", interval_ms=250)

        self.assertIsInstance(source, MockDataSource)
        self.assertEqual(source.interval_ms, 250)

    def test_unsupported_mode_raises(self):
        with self.assertRaises(ValueError):
            create_datasource(mode="serial")


if __name__ == "__main__":
    unittest.main()
