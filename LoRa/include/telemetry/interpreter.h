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
#include <vector>

#include "telemetry/simulation_payloads.h"

// REQUIRED: telemetry-message.pb.h must be generated via nanopb compiler.
// If this header is missing, the build MUST fail to prevent silent fallback behavior.
// Re-generate via: nanopb_generator LoRa/docs/telemetry-message.proto -d LoRa/include/telemetry
#include "telemetry/telemetry-message.pb.h"

namespace Telemetry {
    class TelemetryInterpreter {
        public:
            /**
             * @brief Attempts to decode a Telemetry payload into a proto-backed message and summary.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @param message Output TelemetryMessage feed decoded from the payload.
             * @param summary Output human-readable summary built from TELEMETRY_PROTO_FIELDS.
             * @param decode_source Output decode source identifier.
             * @return True when a payload was decoded successfully via protobuf.
             */
            static bool decode(const uint8_t *payload,
                               size_t payload_len,
                               TelemetryMessage &message,
                               std::string &summary,
                               std::string &decode_source);

            /**
             * @brief Returns a deterministic protobuf payload used by simulation tests.
             * @return Encoded protobuf payload bytes.
             */
            static std::vector<std::uint8_t> simulation_mock_payload();

            /**
             * @brief Indicates whether nanopb Telemetry decode support is compiled in.
             * @return Always true; fallback is no longer supported.
             */
            static bool nanopb_enabled() noexcept;

    };
    
        /**
         * Compatibility struct for legacy `DecodedTelemetry` usages.
         * Some compilation units (or merged branches) still reference
         * `telemetry::DecodedTelemetry` or unqualified `DecodedTelemetry`.
         * Provide a lightweight type here to avoid widespread refactors
         * and restore CI buildability. Fields mirror the common summary
         * fields used by publishers/serializers.
         */
        struct DecodedTelemetry {
            bool decoded{false};
            int mode{0};
            int altitude_m{0};
            int velocity_cms{0};
            int battery_percent{0};
            std::string decode_source;
            std::string summary;
        };


    // Provide lowercase `telemetry` namespace alias and an unqualified alias
    // so code referencing older names still compiles.
    namespace telemetry = Telemetry;
    using DecodedTelemetry = Telemetry::DecodedTelemetry;
}
