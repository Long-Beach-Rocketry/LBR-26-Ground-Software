/**
 * @file LoRa/include/telemetry/test_payloads.h
 * @brief Pre-encoded protobuf telemetry payloads for unit tests
 */

#pragma once

#include <cstdint>
#include<vector>

namespace Telemetry::Test {
    /**
     * @brief Payload: field_1=2, field_2=16, field_3=500, field_4=87
     * Encoded as little-endian uint32 values (16 bytes total)
     */
    inline std::vector<std::uint8_t> payload_mode_2_altitude_16_velocity_500_battery_87() {
        return {
            0x02, 0x00, 0x00, 0x00,  // field_1 = 2 (LE)
            0x10, 0x00, 0x00, 0x00,  // field_2 = 16 (LE)
            0xF4, 0x01, 0x00, 0x00,  // field_3 = 500 (LE)
            0x57, 0x00, 0x00, 0x00   // field_4 = 87 (LE)
        };
    }

    /**
     * @brief Payload: all fields = 0
     */
    inline std::vector<std::uint8_t> payload_all_zeros() {
        return {
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };
    }

    /**
     * @brief Payload: all fields = max uint32
     */
    inline std::vector<std::uint8_t> payload_all_max() {
        return {
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF
        };
    }

    /**
     * @brief Payload: field_1=42, field_2=0x1234 (LE), field_3=0x5678 (LE), field_4=150
     */
    inline std::vector<std::uint8_t> payload_arbitrary_values() {
        return {
            0x2A, 0x00, 0x00, 0x00,  // field_1 = 42
            0x34, 0x12, 0x00, 0x00,  // field_2 = 0x1234 LE
            0x78, 0x56, 0x00, 0x00,  // field_3 = 0x5678 LE
            0x96, 0x00, 0x00, 0x00   // field_4 = 150
        };
    }

    /**
     * @brief Short payload (6 bytes) - invalid for proto, would have been legacy FDCAN
     */
    inline std::vector<std::uint8_t> payload_too_short() {
        return {0x01,0x02, 0x03, 0x04, 0x05, 0x06};
    }

    /**
     * @brief Empty payload
     */
    inline std::vector<std::uint8_t> payload_empty() {
        return {};
    }
}  // namespace Telemetry::Test
