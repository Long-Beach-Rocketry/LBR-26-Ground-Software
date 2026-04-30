/**
 * @file LoRa/src/telemetry/decoded_telemetry_serializer.cc
 * @brief Serializer implementation for DecodedTelemetry payloads
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/decoded_telemetry_serializer.h"

#include "telemetry/telemetry_fields.h"

#include <sstream>
#include <string>

namespace {
    /**
     * @brief Escapes special characters in string for safe JSON embedding
     */
    std::string escape_json_string(const std::string &value) {
        std::string escaped;
        escaped.reserve(value.size());
        for (const char ch : value) {
            switch (ch) {
                case '"':
                    escaped += "\\\"";
                    break;
                case '\\':
                    escaped += "\\\\";
                    break;
                case '\b':
                    escaped += "\\b";
                    break;
                case '\f':
                    escaped += "\\f";
                    break;
                case '\n':
                    escaped += "\\n";
                    break;
                case '\r':
                    escaped += "\\r";
                    break;
                case '\t':
                    escaped += "\\t";
                    break;
                default:
                    escaped += ch;
                    break;
            }
        }

        return escaped;
    }

    /**
     * @brief Escapes double quotes and wraps field in quotes for safe CSV embedding
     */
    std::string escape_csv_field(const std::string &value) {
        std::string escaped;
        escaped.reserve(value.size());
        for (const char ch : value) {
            if (ch == '"')
                escaped += '"';
            escaped += ch;
        }
        return '"' + escaped + '"';
    }

    /**
     * @brief Generically serializes field pairs to JSON.
     * 
     * Takes a vector of (name, value) pairs and produces JSON output.
     * Values are automatically escaped for JSON safety.
     */
    std::string serialize_to_json(const std::vector<std::pair<std::string_view, std::string_view>> &fields) {
        std::ostringstream json;
        json << "{";

        bool first = true;
        for (const auto &field : fields) {
            if (!first)
                json << ",";
            json << "\"" << field.first << "\":\"" << escape_json_string(std::string(field.second)) << "\"";
            first = false;
        }

        json << "}";
        return json.str();
    }

    /**
     * @brief Generically serializes values to CSV.
     * 
     * Takes a vector of values and produces CSV output.
     * Each value is escaped and wrapped in quotes.
     */
    std::string serialize_to_csv(const std::vector<std::string_view> &values) {
        std::ostringstream csv;

        bool first = true;
        for (const auto &value : values) {
            if (!first)
                csv << ",";
            csv << escape_csv_field(std::string(value));
            first = false;
        }

        return csv.str();
    }
}


std::string Telemetry::DecodedTelemetrySerializer::to_json() const {
    /**
     * AUTOMATICALLY GENERATED FROM TELEMETRY_PROTO_FIELDS:
     * This serialization dynamically pulls all proto fields from the central registry.
     * When proto is extended, no changes needed here — just add entry to TELEMETRY_PROTO_FIELDS.
     */
    std::vector<std::string> proto_values;
    proto_values.reserve(Telemetry::TELEMETRY_PROTO_FIELDS.size());

    // Build field values from proto field registry
    for (const auto &field_info : Telemetry::TELEMETRY_PROTO_FIELDS) {
        proto_values.push_back(field_info.to_string_fn(data));
    }

    std::vector<std::pair<std::string_view, std::string_view>> fields;
    fields.reserve(Telemetry::TELEMETRY_PROTO_FIELDS.size() + 3);

    // Map proto field names to their computed values
    for (std::size_t i = 0; i < Telemetry::TELEMETRY_PROTO_FIELDS.size(); ++i) {
        fields.push_back({Telemetry::TELEMETRY_PROTO_FIELDS[i].name, proto_values[i]});
    }

    // Add extra non-proto metadata fields
    const std::string decoded_str = data.decoded ? "true" : "false";
    fields.push_back({"decoded", decoded_str});
    fields.push_back({"decode_source", data.decode_source});
    fields.push_back({"summary", data.summary});

    return serialize_to_json(fields);
}

std::string Telemetry::DecodedTelemetrySerializer::to_csv() const {
    /**
     * AUTOMATICALLY GENERATED FROM TELEMETRY_PROTO_FIELDS:
     * This serialization dynamically pulls all proto fields from the central registry.
     * When proto is extended, no changes needed here — just add entry to TELEMETRY_PROTO_FIELDS.
     */
    std::vector<std::string> values;
    values.reserve(Telemetry::TELEMETRY_PROTO_FIELDS.size() + 3);

    // CSV starts with decoded flag
    values.push_back(data.decoded ? "1" : "0");

    // Add all proto field values in order
    for (const auto &field_info : Telemetry::TELEMETRY_PROTO_FIELDS) {
        values.push_back(field_info.to_string_fn(data));
    }

    // Add extra metadata fields
    values.push_back(data.decode_source);
    values.push_back(data.summary);

    // Convert to string_view vector for serializer
    std::vector<std::string_view> csv_values;
    csv_values.reserve(values.size());
    for (const auto &v : values) {
        csv_values.push_back(v);
    }

    return serialize_to_csv(csv_values);
}
