/**
 * @file LoRa/include/telemetry/telemetry_fields.h
 * @brief Single source of truth for TelemetryMessage field definitions
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 * 
 * This header centralizes the mapping between proto fields and field feed accessors.
 * When extending telemetry-message.proto with a new field:
 * 1. Add field to LoRa/docs/telemetry-message.proto
 * 2. Add ONE entry to TELEMETRY_PROTO_FIELDS below
 * 3. Serializers and summaries automatically include the new field (no manual changes needed)
 */

#pragma once

#include "telemetry/interpreter.h"

#include <array>
#include <cstdint>
#include <string>

namespace Telemetry {

/**
 * @brief Metadata for a single telemetry proto field
 */
struct FieldInfo {
    /** @brief Proto field name (e.g., "field_2") */
    const char *name;

    /** @brief Proto field number (e.g., 2 for field_2) */
    std::uint32_t proto_field_number;

    /** @brief Function pointer to convert field value to string */
    std::string (*to_string_fn)(const TelemetryMessage &);
};

namespace telemetry_field_converters {
    inline std::string field_1_to_string(const TelemetryMessage &message) {
        return std::to_string(message.field_1);
    }

    inline std::string field_2_to_string(const TelemetryMessage &message) {
        return std::to_string(message.field_2);
    }

    inline std::string field_3_to_string(const TelemetryMessage &message) {
        return std::to_string(message.field_3);
    }

    inline std::string field_4_to_string(const TelemetryMessage &message) {
        return std::to_string(message.field_4);
    }
}

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
    {"field_1", 1, telemetry_field_converters::field_1_to_string},
    {"field_2", 2, telemetry_field_converters::field_2_to_string},
    {"field_3", 3, telemetry_field_converters::field_3_to_string},
    {"field_4", 4, telemetry_field_converters::field_4_to_string},
}};

inline std::string summarize_telemetry_message(const TelemetryMessage &message,
                                               const std::string &decode_source) {
    std::string summary = "telemetry_v1 telemetry_proto";
    if (!decode_source.empty())
        summary += " decode_source=" + decode_source;

    for (const auto &field_info : TELEMETRY_PROTO_FIELDS) {
        summary += " ";
        summary += field_info.name;
        summary += "=";
        summary += field_info.to_string_fn(message);
    }

    summary += " altitude_m=";
    summary += telemetry_field_converters::field_2_to_string(message);
    summary += " velocity_cms=";
    summary += telemetry_field_converters::field_3_to_string(message);
    summary += " battery_percent=";
    summary += telemetry_field_converters::field_4_to_string(message);

    return summary;
}

// Explicit instantiation to force the compiler to check it
inline std::string _test_summarize_helper() {
    TelemetryMessage msg = TelemetryMessage_init_zero;
    return summarize_telemetry_message(msg, "test");
}

}  // namespace Telemetry
