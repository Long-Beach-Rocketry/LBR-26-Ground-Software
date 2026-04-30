/**
 * @file LoRa/include/telemetry/telemetry_fields.h
 * @brief Single source of truth for TelemetryMessage field definitions
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 * 
 * This header centralizes the mapping between proto fields and C++ struct fields.
 * When extending telemetry-message.proto with a new field:
 * 1. Add field to LoRa/docs/telemetry-message.proto
 * 2. Add field to Telemetry::DecodedTelemetry struct in LoRa/include/telemetry/interpreter.h
 * 3. Add ONE entry to TELEMETRY_PROTO_FIELDS below
 * 4. Serializers automatically include the new field (no manual changes needed)
 */

#pragma once

#include "telemetry/interpreter.h"

#include <array>
#include <cstdint>
#include <functional>
#include <string>
#include <string_view>

namespace Telemetry {

/**
 * @brief Metadata for a single telemetry proto field
 */
struct FieldInfo {
    /** @brief Proto field name (e.g., "altitude_m") */
    const char *name;

    /** @brief Proto field number (e.g., 2 for altitude_m) */
    std::uint32_t proto_field_number;

    /** @brief Function to convert field value to string */
    std::function<std::string(const DecodedTelemetry &)> to_string_fn;
};

/**
 * @brief THE SINGLE SOURCE OF TRUTH for proto field definitions.
 * 
 * This array defines which fields are serialized and in what order.
 * When the reviewer extends telemetry-message.proto, add the new field's
 * metadata here (one entry) and it automatically flows through JSON/CSV serialization.
 * 
 * Order matters: the first entry corresponds to proto field 1, second to field 2, etc.
 */
inline const std::array<FieldInfo, 4> TELEMETRY_PROTO_FIELDS{{
    {"mode", 1, [](const DecodedTelemetry &d) { return std::to_string(d.mode); }},
    {"altitude_m", 2, [](const DecodedTelemetry &d) { return std::to_string(d.altitude_m); }},
    {"velocity_cms", 3, [](const DecodedTelemetry &d) { return std::to_string(d.velocity_cms); }},
    {"battery_percent", 4, [](const DecodedTelemetry &d) { return std::to_string(d.battery_percent); }},
}};

}  // namespace Telemetry
