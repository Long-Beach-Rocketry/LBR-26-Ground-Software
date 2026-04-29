/**
 * @file interpreter.h
 * @brief Lightweight telemetry payload interpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_TELEMETRY_INTERPRETER_H_
#define LORA_INCLUDE_TELEMETRY_INTERPRETER_H_

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace telemetry {
    struct DecodedTelemetry {
        bool decoded = false;
        std::string summary;
        std::uint32_t mode = 0;
        std::uint32_t altitude_m = 0;
        std::uint32_t velocity_cms = 0;
        std::uint32_t battery_percent = 0;
        std::string decode_source = "unknown";  // decode source (fallback_fdcan, pb_decode, error, etc.)
    };

    class TelemetryInterpreter {
        public:
            /**
             * @brief Attempts to decode a telemetry payload into a human-readable summary.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @return Decode status and summary text.
             */
            static DecodedTelemetry decode(const uint8_t *payload, size_t payload_len);

            /**
             * @brief Decodes legacy FDCAN telemetry payload format.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @return Decode status and summary text.
             */
            static DecodedTelemetry fallback_decode(const uint8_t *payload, size_t payload_len);

            /**
             * @brief Returns a deterministic protobuf payload used by simulation tests.
             * @return Encoded protobuf payload bytes.
             */
            static std::vector<std::uint8_t> simulation_mock_payload();

            /**
             * @brief Indicates whether nanopb telemetry decode support is compiled in.
             * @return True when pb_decode based path is available.
             */
            static bool nanopb_enabled() noexcept;

        private:
            static bool is_fdcan_legacy_payload(size_t payload_len) noexcept;
    };

    using Interpreter = TelemetryInterpreter;
}

#endif  // LORA_INCLUDE_TELEMETRY_INTERPRETER_H_
