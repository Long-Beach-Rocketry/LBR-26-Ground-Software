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

namespace telemetry {
    struct DecodedTelemetry {
        bool decoded = false;
        std::string summary;
    };

    class Interpreter {
        public:
            /**
             * @brief Attempts to decode a telemetry payload into a human-readable summary.
             * @param payload Pointer to payload bytes.
             * @param payload_len Number of payload bytes.
             * @return Decode status and summary text.
             */
            static DecodedTelemetry decode(const uint8_t *payload, size_t payload_len);
    };
}

#endif  // LORA_INCLUDE_TELEMETRY_INTERPRETER_H_
