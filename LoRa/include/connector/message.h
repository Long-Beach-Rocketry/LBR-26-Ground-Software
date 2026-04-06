/**
 * @file message.h
 * @brief Versioned connector message model and JSON serialization helpers
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstdint>
#include <map>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace connector {
    /**
     * @brief Logical source identifier used by the connector contract.
     */
    enum class Source {
        Sx1262,
        Sx127,
        Simulated
    };

    /**
     * @brief Converts a source enum value to its contract string representation.
     * @param source Source enum value.
     * @return Lowercase string name used in connector JSON messages.
     */
    std::string source_to_string(Source source);

    /**
     * @brief Parses a source enum value from its contract string representation.
     * @param source_name Lowercase source name from a connector JSON message.
     * @return Parsed source enum value.
     * @throws std::runtime_error when the source name is not supported.
     */
    Source source_from_string(std::string_view source_name);

    /**
     * @brief Application-level connector payload exchanged with the consumer side.
     */
    struct ConnectorMessage {
        int schema_version = 1;
        std::string message_type = "telemetry_frame";
        std::uint64_t sequence = 0;
        std::int64_t timestamp_ms = 0;
        Source source = Source::Simulated;
        std::vector<std::uint8_t> payload;
        std::optional<std::string> checksum_hex;
        std::map<std::string, std::string> metadata;

        /**
         * @brief Serializes this message into a JSON object string matching the connector contract.
         * @return JSON string for one connector message.
         * @throws std::runtime_error when the schema version or message type is unsupported.
         */
        std::string to_json() const;

        /**
         * @brief Parses a connector message from a JSON object string.
         * @param json_text JSON object text matching the connector contract.
         * @return Parsed connector message.
         * @throws std::runtime_error when required fields are missing or malformed.
         */
        static ConnectorMessage from_json(const std::string &json_text);
    };
}
