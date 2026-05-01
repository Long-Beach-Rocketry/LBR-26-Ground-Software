/**
 * @file LoRa/src/telemetry/decoded_telemetry_serializer.cc
 * @brief Serializer implementation for TelemetryMessage payloads
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/decoded_telemetry_serializer.h"

#include "telemetry/telemetry_fields.h"

#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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


std::string Telemetry::TelemetryMessageSerializer::to_json() const {
    std::vector<std::pair<std::string_view, std::string_view>> fields;
    fields.reserve(Telemetry::TELEMETRY_PROTO_FIELDS.size());

    for (const auto &field_info : Telemetry::TELEMETRY_PROTO_FIELDS) {
        fields.push_back({field_info.name, field_info.to_string_fn(data)});
    }

    return serialize_to_json(fields);
}

std::string Telemetry::TelemetryMessageSerializer::to_csv() const {
    std::vector<std::string> csv_values;
    csv_values.reserve(Telemetry::TELEMETRY_PROTO_FIELDS.size());

    for (const auto &field_info : Telemetry::TELEMETRY_PROTO_FIELDS) {
        csv_values.push_back(field_info.to_string_fn(data));
    }

    return serialize_to_csv(csv_values);
}
