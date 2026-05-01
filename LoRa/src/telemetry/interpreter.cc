/**
 * @file LoRa/src/telemetry/interpreter.cc
 * @brief Lightweight Telemetry payload interpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/interpreter.h"
#include "telemetry/simulation_payloads.h"
#include "telemetry/telemetry_fields.h"

#include <pb_decode.h>
#include "telemetry-message.pb.h"

#include <sstream>

namespace {
    /**
     * @brief Maximum allowed telemetry payload size in bytes (256 bytes)
     */
    constexpr std::size_t kMaxTelemetryPayloadBytes = 256U;
}

bool Telemetry::TelemetryInterpreter::decode(const uint8_t *payload,
                                            size_t payload_len,
                                            TelemetryMessage &message,
                                            std::string &summary,
                                            std::string &decode_source) {
    if (payload == nullptr || payload_len == 0) {
        decode_source = "error";
        summary = "null or empty payload";
        return false;
    }

    if (payload_len > kMaxTelemetryPayloadBytes) {
        decode_source = "error";
        summary = "payload exceeds max size";
        return false;
    }

    pb_istream_t stream = pb_istream_from_buffer(payload, payload_len);
    if (pb_decode(&stream, TelemetryMessage_fields, &message)) {
        decode_source = "pb_decode";
        summary = summarize_telemetry_message(message, decode_source);
        return true;
    }

    decode_source = "error";
    summary = "protobuf decode failed";
    return false;
}

bool Telemetry::TelemetryInterpreter::nanopb_enabled() noexcept {
    return true;  // nanopb is now required; fallback is no longer supported.
}

std::vector<std::uint8_t> Telemetry::TelemetryInterpreter::simulation_mock_payload() {
    return simulation_mock_payload_bytes();
}
