"""
@File:   factory.py
@Brief:  Data source factory helpers
@Author: Luis Fernandes
@Orgin:  Long Beach Rocketry

Description:
    Factory function used by the GUI bootstrap to instantiate a data source
    implementation from a mode string.
"""

from data.mockdata import MockDataSource


def create_datasource(mode="mock", interval_ms=1000):
    normalized_mode = str(mode or "mock").strip().lower()

    if normalized_mode == "mock":
        return MockDataSource(interval_ms=interval_ms)

    raise ValueError(
        f"Unsupported data source mode '{mode}'. Supported modes: mock"
    )
