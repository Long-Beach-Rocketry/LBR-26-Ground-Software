/**
 * @file message.cc
 * @brief Versioned connector message model and JSON serialization helpers
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/message.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <yaml-cpp/yaml.h>

namespace {
    std::string escape_json_string(const std::string &input) {
        std::ostringstream output;
        output << '"';
        for (const char character : input) {
            switch (character) {
                case '\\': output << "\\\\"; break;
                case '"': output << "\\\""; break;
                case '\b': output << "\\b"; break;
                case '\f': output << "\\f"; break;
                case '\n': output << "\\n"; break;
                case '\r': output << "\\r"; break;
                case '\t': output << "\\t"; break;
                default:
                    if (static_cast<unsigned char>(character) < 0x20) {
                        output << "\\u"
                               << std::hex << std::uppercase << std::setw(4)
                               << std::setfill('0')
                               << static_cast<int>(static_cast<unsigned char>(character));
                    } else {
                        output << character;
                    }
            }
        }
        output << '"';
        return output.str();
    }

    std::string base64_encode(const std::vector<std::uint8_t> &bytes) {
        static constexpr char alphabet[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        std::string output;
        output.reserve(((bytes.size() + 2) / 3) * 4);

        std::uint32_t accumulator = 0;
        int bit_count = -6;
        for (const std::uint8_t byte : bytes) {
            accumulator = (accumulator << 8) | byte;
            bit_count += 8;
            while (bit_count >= 0) {
                output.push_back(alphabet[(accumulator >> bit_count) & 0x3F]);
                bit_count -= 6;
            }
        }

        if (bit_count > -6)
            output.push_back(alphabet[((accumulator << 8) >> (bit_count + 8)) & 0x3F]);

        while (output.size() % 4 != 0)
            output.push_back('=');

        return output;
    }

    std::vector<std::uint8_t> base64_decode(const std::string &encoded) {
        static constexpr std::string_view alphabet =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        auto decode_value = [](unsigned char character) -> int {
            if (character >= 'A' && character <= 'Z')
                return character - 'A';
            if (character >= 'a' && character <= 'z')
                return character - 'a' + 26;
            if (character >= '0' && character <= '9')
                return character - '0' + 52;
            if (character == '+')
                return 62;
            if (character == '/')
                return 63;
            return -1;
        };

        std::vector<std::uint8_t> output;
        std::uint32_t accumulator = 0;
        int bit_count = -8;
        for (const unsigned char character : encoded) {
            if (std::isspace(character))
                continue;
            if (character == '=')
                break;

            const int decoded_value = decode_value(character);
            if (decoded_value < 0)
                throw std::runtime_error("Invalid base64 payload.");

            accumulator = (accumulator << 6) | static_cast<std::uint32_t>(decoded_value);
            bit_count += 6;
            if (bit_count >= 0) {
                output.push_back(static_cast<std::uint8_t>((accumulator >> bit_count) & 0xFF));
                bit_count -= 8;
            }
        }

        return output;
    }

    template <typename T>
    T read_required_scalar(const YAML::Node &root, const char *field_name) {
        const YAML::Node node = root[field_name];
        if (!node)
            throw std::runtime_error(std::string("Missing field: ") + field_name);
        if (!node.IsScalar())
            throw std::runtime_error(std::string("Field must be scalar: ") + field_name);

        try {
            return node.as<T>();
        } catch (const YAML::Exception &e) {
            throw std::runtime_error(std::string("Invalid field '") + field_name + "': " + e.what());
        }
    }
}

std::string connector::source_to_string(Source source) {
    switch (source) {
        case Source::Sx1262: return "sx1262";
        case Source::Sx127: return "sx127";
        case Source::Simulated: return "simulated";
    }

    return "simulated";
}

connector::Source connector::source_from_string(std::string_view source_name) {
    if (source_name == "sx1262")
        return Source::Sx1262;
    if (source_name == "sx127")
        return Source::Sx127;
    if (source_name == "simulated")
        return Source::Simulated;

    throw std::runtime_error("Unknown connector source: " + std::string(source_name));
}

std::string connector::ConnectorMessage::to_json() const {
    if (schema_version < 1)
        throw std::runtime_error("schema_version must be >= 1.");
    if (message_type.empty())
        throw std::runtime_error("message_type must not be empty.");
    if (source.empty())
        throw std::runtime_error("source must not be empty.");

    std::ostringstream output;
    output << '{'
           << "\"schema_version\":" << schema_version << ','
           << "\"message_type\":" << escape_json_string(message_type) << ','
           << "\"sequence\":" << sequence << ','
           << "\"timestamp_ms\":" << timestamp_ms << ','
           << "\"source\":" << escape_json_string(source) << ','
           << "\"payload_b64\":" << escape_json_string(base64_encode(payload));

    if (checksum_hex.has_value())
        output << ',' << "\"checksum_hex\":" << escape_json_string(*checksum_hex);

    if (!metadata.empty()) {
        output << ',' << "\"metadata\":{";
        bool first = true;
        for (const auto &entry : metadata) {
            if (!first)
                output << ',';
            first = false;
            output << escape_json_string(entry.first) << ':' << escape_json_string(entry.second);
        }
        output << '}';
    }

    output << '}';
    return output.str();
}

connector::ConnectorMessage connector::ConnectorMessage::from_json(const std::string &json_text) {
    YAML::Node root;
    try {
        root = YAML::Load(json_text);
    } catch (const YAML::Exception &e) {
        throw std::runtime_error(std::string("Malformed connector JSON: ") + e.what());
    }

    if (!root || !root.IsMap())
        throw std::runtime_error("Connector JSON root must be an object.");

    ConnectorMessage message;
    message.schema_version = static_cast<int>(read_required_scalar<std::uint64_t>(root, "schema_version"));
    if (message.schema_version < 1)
        throw std::runtime_error("schema_version must be >= 1.");

    message.message_type = read_required_scalar<std::string>(root, "message_type");
    if (message.message_type.empty())
        throw std::runtime_error("message_type must not be empty.");

    message.sequence = read_required_scalar<std::uint64_t>(root, "sequence");
    message.timestamp_ms = read_required_scalar<std::int64_t>(root, "timestamp_ms");
    message.source = read_required_scalar<std::string>(root, "source");
    if (message.source.empty())
        throw std::runtime_error("source must not be empty.");

    message.payload = base64_decode(read_required_scalar<std::string>(root, "payload_b64"));

    const YAML::Node checksum_node = root["checksum_hex"];
    if (checksum_node) {
        if (!checksum_node.IsScalar())
            throw std::runtime_error("checksum_hex must be a string.");
        const std::string checksum = checksum_node.as<std::string>();
        if (!checksum.empty())
            message.checksum_hex = checksum;
    }

    const YAML::Node metadata_node = root["metadata"];
    if (metadata_node) {
        if (!metadata_node.IsMap())
            throw std::runtime_error("metadata must be an object.");

        for (const auto &entry : metadata_node) {
            if (!entry.first.IsScalar() || !entry.second.IsScalar())
                throw std::runtime_error("metadata keys and values must be strings.");

            message.metadata.emplace(entry.first.as<std::string>(), entry.second.as<std::string>());
        }
    }

    return message;
}
