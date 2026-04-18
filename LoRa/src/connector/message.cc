/**
 * @file message.cc
 * @brief Versioned connector message model and JSON serialization helpers
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/message.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace {
    std::string field_prefix(const std::string &field_name) {
        return std::string("\"") + field_name + "\"\\s*:\\s*";
    }

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

    std::string read_string_field(const std::string &json_text, const std::string &field_name) {
        const std::regex pattern(field_prefix(field_name) + R"REGEX("((?:\\.|[^"])*)")REGEX");
        std::smatch match;
        if (!std::regex_search(json_text, match, pattern) || match.size() < 2)
            throw std::runtime_error("Missing string field: " + field_name);
        return match[1].str();
    }

    std::string unescape_json_string(const std::string &input) {
        std::string output;
        output.reserve(input.size());
        for (std::size_t index = 0; index < input.size(); ++index) {
            const char character = input[index];
            if (character != '\\') {
                output.push_back(character);
                continue;
            }

            if (index + 1 >= input.size())
                throw std::runtime_error("Invalid escape sequence in JSON string.");

            const char escaped = input[++index];
            switch (escaped) {
                case '\\': output.push_back('\\'); break;
                case '"': output.push_back('"'); break;
                case 'b': output.push_back('\b'); break;
                case 'f': output.push_back('\f'); break;
                case 'n': output.push_back('\n'); break;
                case 'r': output.push_back('\r'); break;
                case 't': output.push_back('\t'); break;
                case 'u':
                    throw std::runtime_error("Unicode escapes are not supported in connector JSON.");
                default:
                    throw std::runtime_error("Unknown escape sequence in JSON string.");
            }
        }
        return output;
    }

    std::string read_optional_string_field(const std::string &json_text,
                                           const std::string &field_name) {
        const std::regex pattern(field_prefix(field_name) + R"REGEX("((?:\\.|[^"])*)")REGEX");
        std::smatch match;
        if (!std::regex_search(json_text, match, pattern) || match.size() < 2)
            return {};
        return unescape_json_string(match[1].str());
    }

    std::uint64_t read_uint64_field(const std::string &json_text, const std::string &field_name) {
        const std::regex pattern(field_prefix(field_name) + R"(([0-9]+))");
        std::smatch match;
        if (!std::regex_search(json_text, match, pattern) || match.size() < 2)
            throw std::runtime_error("Missing integer field: " + field_name);
        return static_cast<std::uint64_t>(std::stoull(match[1].str()));
    }

    std::int64_t read_int64_field(const std::string &json_text, const std::string &field_name) {
        const std::regex pattern(field_prefix(field_name) + R"((-?[0-9]+))");
        std::smatch match;
        if (!std::regex_search(json_text, match, pattern) || match.size() < 2)
            throw std::runtime_error("Missing integer field: " + field_name);
        return static_cast<std::int64_t>(std::stoll(match[1].str()));
    }

    std::string extract_object_text(const std::string &json_text, const std::string &field_name) {
        const std::string key = '"' + field_name + '"';
        const std::size_t key_position = json_text.find(key);
        if (key_position == std::string::npos)
            return {};

        const std::size_t brace_position = json_text.find('{', key_position + key.size());
        if (brace_position == std::string::npos)
            throw std::runtime_error("Malformed object field: " + field_name);

        int depth = 0;
        for (std::size_t index = brace_position; index < json_text.size(); ++index) {
            if (json_text[index] == '{')
                ++depth;
            else if (json_text[index] == '}') {
                --depth;
                if (depth == 0)
                    return json_text.substr(brace_position + 1, index - brace_position - 1);
            }
        }

        throw std::runtime_error("Unterminated object field: " + field_name);
    }

    std::map<std::string, std::string> parse_metadata_object(const std::string &json_text) {
        std::map<std::string, std::string> metadata;
        if (json_text.empty())
            return metadata;

        const std::regex pattern(R"REGEX("((?:\\.|[^"])*)"\s*:\s*"((?:\\.|[^"])*)")REGEX");
        for (std::sregex_iterator it(json_text.begin(), json_text.end(), pattern), end; it != end;
             ++it) {
            metadata.emplace(unescape_json_string((*it)[1].str()), unescape_json_string((*it)[2].str()));
        }
        return metadata;
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
    ConnectorMessage message;
    message.schema_version = static_cast<int>(read_uint64_field(json_text, "schema_version"));
    if (message.schema_version < 1)
        throw std::runtime_error("schema_version must be >= 1.");

    message.message_type = read_string_field(json_text, "message_type");
    if (message.message_type.empty())
        throw std::runtime_error("message_type must not be empty.");

    message.sequence = read_uint64_field(json_text, "sequence");
    message.timestamp_ms = read_int64_field(json_text, "timestamp_ms");
    message.source = read_string_field(json_text, "source");
    if (message.source.empty())
        throw std::runtime_error("source must not be empty.");

    message.payload = base64_decode(read_string_field(json_text, "payload_b64"));

    const std::string checksum = read_optional_string_field(json_text, "checksum_hex");
    if (!checksum.empty())
        message.checksum_hex = checksum;

    const std::string metadata_text = extract_object_text(json_text, "metadata");
    if (!metadata_text.empty())
        message.metadata = parse_metadata_object(metadata_text);

    return message;
}
