"""
@File:   alert_rules.py
@Brief:  Alert strategy primitives for telemetry cards
@Author: Luis Fernandes
@Orgin:  Long Beach Rocketry

Description:
    Strategy objects used by the telemetry cards panel to evaluate
    per-metric alert conditions.
"""


class AlertRule:
    def is_alert(self, value):
        raise NotImplementedError()


class GreaterThanRule(AlertRule):
    def __init__(self, threshold):
        self._threshold = threshold

    def is_alert(self, value):
        return (value is not None) and (value > self._threshold)


class LessThanRule(AlertRule):
    def __init__(self, threshold):
        self._threshold = threshold

    def is_alert(self, value):
        return (value is not None) and (value < self._threshold)


class NeverAlertRule(AlertRule):
    def is_alert(self, value):
        return False


def default_alert_rules():
    return {
        "temp": GreaterThanRule(80.0),
        "battery": LessThanRule(5.0),
        "pressure": LessThanRule(50000.0),
        "signal": LessThanRule(-90.0),
    }
