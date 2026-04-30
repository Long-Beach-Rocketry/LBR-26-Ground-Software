/**
 * @file LoRa/src/telemetry/interpreter.cc
 * @brief Lightweight Telemetry payload interpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/interpreter.h"
#include "telemetry/simulation_payloads.h"

#include <sstream>

#if defined(__has_include)
    #if __has_include(<pb_decode.h>) && __has_include("Telemetry-message.pb.h")
        #include <pb_decode.h>
        #include "telemetry-message.pb.h"
        #define LBR_TELEMETRY_HAS_NANOPB 1
    #else
        #define LBR_TELEMETRY_HAS_NANOPB 0
    #endif
#else
    #define LBR_TELEMETRY_HAS_NANOPB 0
#endif

namespace {
    /**
     * @brief Maximum allowed telemetry payload size in bytes (256 bytes)
     */
    constexpr std::size_t kMaxTelemetryPayloadBytes = 256U;
}

Telemetry::DecodedTelemetry Telemetry::TelemetryInterpreter::decode(const uint8_t *payload,
                                                                    size_t payload_len) {
    if (payload == nullptr)
        return make_decoded_telemetry(false, "missing payload buffer", 0, 0, 0, 0, "error");
    if (payload_len == 0)
        return make_decoded_telemetry(false, "empty payload", 0, 0, 0, 0, "error");
    if (payload_len > kMaxTelemetryPayloadBytes)
        return make_decoded_telemetry(false, "payload too large", 0, 0, 0, 0, "error");

#if LBR_TELEMETRY_HAS_NANOPB
    TelemetryMessage message = TelemetryMessage_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(payload, payload_len);
    if (pb_decode(&stream, TelemetryMessage_fields, &message)) {
        std::ostringstream summary;
        summary << "telemetry_pb_v1"
                << " mode=" << static_cast<int>(message.mode)
                << " altitude_m=" << static_cast<unsigned int>(message.altitude_m)
                << " velocity_cms=" << static_cast<unsigned int>(message.velocity_cms)
                << " battery_percent=" << static_cast<int>(message.battery_percent);
        return make_decoded_telemetry(true, summary.str(), message.mode, message.altitude_m,
                                      message.velocity_cms, message.battery_percent, "pb_decode");
    }
#endif

    if (is_fdcan_legacy_payload(payload_len))
        return fallback_decode(payload, payload_len);

    return make_decoded_telemetry(false, "nanopb decode failed and payload is not legacy fdcan", 0, 0, 0, 0, "error");
}

Telemetry::DecodedTelemetry Telemetry::TelemetryInterpreter::fallback_decode(const uint8_t *payload,
                                                                             size_t payload_len) {
    if (payload == nullptr)
        return make_decoded_telemetry(false, "missing payload buffer", 0, 0, 0, 0, "error");
    if (!is_fdcan_legacy_payload(payload_len))
        return make_decoded_telemetry(false, "payload is not legacy fdcan telemetry_v1", 0, 0, 0, 0, "error");

    const uint8_t mode = payload[0];
    const uint16_t altitude_m =
        static_cast<uint16_t>(payload[1]) | (static_cast<uint16_t>(payload[2]) << 8);
    const uint16_t velocity_cms =
        static_cast<uint16_t>(payload[3]) | (static_cast<uint16_t>(payload[4]) << 8);
    const uint8_t battery_percent = payload[5];

    std::ostringstream summary;
    summary << "telemetry_v1"
            << " decode_source=fallback_fdcan"
            << " mode=" << static_cast<int>(mode)
            << " altitude_m=" << altitude_m
            << " velocity_cms=" << velocity_cms
            << " battery_percent=" << static_cast<int>(battery_percent);

    return make_decoded_telemetry(true, summary.str(), mode, altitude_m, velocity_cms, battery_percent, "fallback_fdcan");
}

bool Telemetry::TelemetryInterpreter::nanopb_enabled() noexcept {
    return LBR_TELEMETRY_HAS_NANOPB != 0;
}

bool Telemetry::TelemetryInterpreter::is_fdcan_legacy_payload(size_t payload_len) noexcept {
    return payload_len == 6U;
}

std::vector<std::uint8_t> Telemetry::TelemetryInterpreter::simulation_mock_payload() {
    return simulation_mock_payload_bytes();
}
