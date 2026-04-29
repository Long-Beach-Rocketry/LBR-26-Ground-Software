/**
 * @file decoded_telemetry_publisher.cc
 * @brief ZeroMQ publisher for decoded telemetry summaries
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/decoded_telemetry_publisher.h"

#include "connector/local_zmq_transport.h"
#include "connector/message.h"

#include <chrono>

telemetry::DecodedTelemetryPublisher::DecodedTelemetryPublisher(std::string endpoint,
                                                                std::string topic)
    : _endpoint(std::move(endpoint)), _topic(std::move(topic)) {}

void telemetry::DecodedTelemetryPublisher::publish(const DecodedTelemetry &decoded,
                                                   const std::string &source,
                                                   std::uint64_t sequence) {
    connector::ConnectorMessage message;
    message.message_type = "telemetry_decoded";
    message.sequence = sequence;
    message.timestamp_ms =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now().time_since_epoch())
            .count();
    message.source = source;
    message.payload.assign(decoded.summary.begin(), decoded.summary.end());
    message.metadata["decoder"] = "TelemetryInterpreter";

    connector::LocalZmqTransport publisher(
        connector::LocalZmqMode::Publisher, _endpoint, _topic);
    publisher.open();
    publisher.send_message(message.to_json());
}
