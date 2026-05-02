/**
 * @file LoRa/include/telemetry/simulation_payloads.h
 * @brief Deterministic Telemetry payload fixtures for simulation paths
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstdint>
#include <vector>

namespace Telemetry {
    /**
     * @brief Returns a deterministic mock payload used by telemetry simulation tests.
     * @return Fixed protobuf-like payload byte sequence.
     */
    std::vector<std::uint8_t> simulation_mock_payload_bytes();
}
