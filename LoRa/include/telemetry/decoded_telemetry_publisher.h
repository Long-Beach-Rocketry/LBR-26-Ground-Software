/**
 * @file decoded_telemetry_publisher.h
 * @brief ZeroMQ publisher for decoded telemetry summaries
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_PUBLISHER_H_
#define LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_PUBLISHER_H_

#include "telemetry/interpreter.h"

#include <cstdint>
#include <string>

namespace telemetry {
    class DecodedTelemetryPublisher {
        public:
            DecodedTelemetryPublisher(std::string endpoint, std::string topic);

            void publish(const DecodedTelemetry &decoded,
                         const std::string &source,
                         std::uint64_t sequence);

        private:
            std::string _endpoint;
            std::string _topic;
    };
}

#endif  // LORA_INCLUDE_TELEMETRY_DECODED_TELEMETRY_PUBLISHER_H_
