/**
 * @file LoRa/archive/connector/checksum.h
 * @brief Archived checksum utilities for connector messages
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace connector {

std::string compute_crc32_hex(const uint8_t* data, size_t size) noexcept;
std::string compute_crc32_hex(const std::string& data) noexcept;
bool validate_crc32(const uint8_t* data, size_t size, const std::string& expected_hex) noexcept;
bool is_valid_crc32_hex(const std::string& checksum_hex) noexcept;

}  // namespace connector