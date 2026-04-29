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
#include <string>

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
    
    // Build structured JSON payload
    std::string payload_json = "{";
    payload_json += "\"decoded\":" + std::string(decoded.decoded ? "true" : "false") + ",";
    payload_json += "\"mode\":" + std::to_string(decoded.mode) + ",";
    payload_json += "\"altitude_m\":" + std::to_string(decoded.altitude_m) + ",";
    payload_json += "\"velocity_cms\":" + std::to_string(decoded.velocity_cms) + ",";
    payload_json += "\"battery_percent\":" + std::to_string(decoded.battery_percent) + ",";
    payload_json += "\"decode_source\":\"" + decoded.decode_source + "\",";
    payload_json += "\"summary\":\"" + decoded.summary + "\"";
    payload_json += "}";
    
    message.payload.assign(payload_json.begin(), payload_json.end());
    message.metadata["decoder"] = "TelemetryInterpreter";

    connector::LocalZmqTransport publisher(
        connector::LocalZmqMode::Publisher, _endpoint, _topic);
    publisher.open();
    publisher.send_message(message.to_json());
}
