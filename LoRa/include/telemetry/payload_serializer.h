/**
 * @file LoRa/include/telemetry/payload_serializer.h
 * @brief Abstract interface for serializing telemetry payloads
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_TELEMETRY_PAYLOAD_SERIALIZER_H_
#define LORA_INCLUDE_TELEMETRY_PAYLOAD_SERIALIZER_H_

#include <string>

namespace Telemetry {
    /**
     * @brief Abstract interface for serializing payload data to JSON and CSV formats.
     *
     * Implementations provide format-agnostic serialization of different telemetry payload types.
     * This allows new board types or payload formats to be added without modifying the publisher.
     */
    class IPayloadSerializer {
        public:
            virtual ~IPayloadSerializer() = default;

            /**
             * @brief Serializes payload data to JSON format.
             * @return JSON string representation of the payload.
             */
            virtual std::string to_json() const = 0;

            /**
             * @brief Serializes payload data to CSV format.
             * @return CSV string representation of the payload.
             */
            virtual std::string to_csv() const = 0;
    };
}

#endif  // LORA_INCLUDE_TELEMETRY_PAYLOAD_SERIALIZER_H_
