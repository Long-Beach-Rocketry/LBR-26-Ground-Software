/**
 * @file LoRa/archive/connector/checksum.cc
 * @brief Archived checksum implementation
 */

#include "checksum.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>

namespace connector {

static std::uint32_t compute_crc32(const uint8_t* data, size_t size) noexcept {
    std::uint32_t crc = 0xFFFFFFFFU;
    for (size_t i = 0; i < size; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            if ((crc & 1U) != 0U)
                crc = (crc >> 1U) ^ 0xEDB88320U;
            else
                crc >>= 1U;
        }
    }
    return crc ^ 0xFFFFFFFFU;
}

std::string compute_crc32_hex(const uint8_t* data, size_t size) noexcept {
    std::uint32_t crc = compute_crc32(data, size);
    std::ostringstream ss;
    ss << std::hex << std::setfill('0') << std::setw(8) << crc;
    return ss.str();
}

std::string compute_crc32_hex(const std::string& data) noexcept {
    return compute_crc32_hex(reinterpret_cast<const uint8_t*>(data.data()), data.size());
}

bool validate_crc32(const uint8_t* data, size_t size, const std::string& expected_hex) noexcept {
    if (!is_valid_crc32_hex(expected_hex))
        return false;

    std::string computed = compute_crc32_hex(data, size);
    return computed.length() == expected_hex.length() &&
           std::equal(computed.begin(), computed.end(), expected_hex.begin(),
                      [](char a, char b) {
                          return std::tolower(static_cast<unsigned char>(a)) ==
                                 std::tolower(static_cast<unsigned char>(b));
                      });
}

bool is_valid_crc32_hex(const std::string& checksum_hex) noexcept {
    if (checksum_hex.size() != 8U)
        return false;

    return std::all_of(checksum_hex.begin(), checksum_hex.end(),
                       [](unsigned char c) { return std::isxdigit(c) != 0; });
}

}  // namespace connector