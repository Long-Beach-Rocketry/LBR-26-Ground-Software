/**
 * @file LoRa/include/telemetry/interpreter.h
 * @brief Lightweight Telemetry payload interpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include "telemetry/simulation_payloads.h"

namespace Telemetry {
    /**
     * @brief Decoded Telemetry frame with structured fields and summary.
     */
    struct DecodedTelemetry {
        bool decoded = false;
        std::string summary;
        std::uint32_t mode = 0;
        std::uint32_t altitude_m = 0;
        std::uint32_t velocity_cms = 0;
        std::uint32_t battery_percent = 0;
        std::string decode_source = "unknown";
    };

    /**
     * @brief Helper to safely construct a DecodedTelemetry from structured values.
     * 
     * This ensures all accesses align with TELEMETRY_PROTO_FIELDS registry (see telemetry_fields.h).
     * When adding a new field:
     * 1. Add it to the TelemetryMessage proto
     * 2. Add it to DecodedTelemetry struct above
     * 3. Add one entry to TELEMETRY_PROTO_FIELDS 
     * 4. Pass it here
     */
    inline DecodedTelemetry make_decoded_telemetry(
        bool decoded,
        std::string_view summary,
        std::uint32_t mode,
        std::uint32_t altitude_m,
        std::uint32_t velocity_cms,
        std::uint32_t battery_percent,
        std::string_view decode_source) noexcept {
        return {decoded, std::string(summary), mode, altitude_m, velocity_cms, battery_percent, std::string(decode_source)};
    }

    class TelemetryInterpreter {
        public:
            /**
             * @brief Attempts to decode a Telemetry payload into a human-readable summary.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @return Decode status and summary text.
             */
            static DecodedTelemetry decode(const uint8_t *payload, size_t payload_len);

            /**
             * @brief Decodes legacy FDCAN Telemetry payload format.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @return Decode status and summary text.
             */
            static DecodedTelemetry fallback_decode(const uint8_t *payload, size_t payload_len);

            /**
             * @brief Returns a deterministic protobuf payload used by simulation Tests.
             * @return Encoded protobuf payload bytes.
             */
            static std::vector<std::uint8_t> simulation_mock_payload();

            /**
             * @brief Indicates whether nanopb Telemetry decode support is compiled in.
             * @return True when pb_decode based path is available.
             */
            static bool nanopb_enabled() noexcept;

        private:
            /**
             * @brief Checks whether the payload matches the legacy FDCAN frame size.
             * @param payload_len Payload length in bytes.
             * @return True when the payload length is the expected legacy frame size.
             */
            static bool is_fdcan_legacy_payload(size_t payload_len) noexcept;
    };

}
