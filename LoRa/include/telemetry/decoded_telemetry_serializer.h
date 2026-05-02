/**
 * @file LoRa/include/telemetry/decoded_telemetry_serializer.h
 * @brief Serializer for TelemetryMessage payloads
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_SERIALIZER_H_
#define LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_SERIALIZER_H_

#include "telemetry/payload_serializer.h"
#include "telemetry/interpreter.h"

namespace Telemetry {
    /**
     * @brief Serializes TelemetryMessage structures to JSON and CSV formats.
     */
    class TelemetryMessageSerializer : public IPayloadSerializer {
        public:
            /**
             * @brief Creates a serializer for the given TelemetryMessage feed.
             * @param message Reference to TelemetryMessage structure.
             */
            explicit TelemetryMessageSerializer(const TelemetryMessage &message)
                : data(message) {}

            /**
             * @brief Serializes to JSON with all telemetry fields.
             * @return JSON string representation.
             */
            std::string to_json() const override;

            /**
             * @brief Serializes to CSV with field values in order.
             * @return CSV string representation.
             */
            std::string to_csv() const override;

        private:
            const TelemetryMessage &data;
    };
}

#endif  // LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_SERIALIZER_H_
