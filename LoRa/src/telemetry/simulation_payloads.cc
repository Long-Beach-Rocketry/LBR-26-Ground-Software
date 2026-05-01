/**
 * @file LoRa/src/telemetry/simulation_payloads.cc
 * @brief Deterministic Telemetry payload fixtures for simulation paths
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/simulation_payloads.h"

namespace Telemetry {
    std::vector<std::uint8_t> simulation_mock_payload_bytes() {
        return {
            0x08U,
            0x02U,
            0x10U,
            0x10U,
            0x18U,
            0xF4U,
            0x03U,
            0x20U,
            0x57U,
        };
    }
}
