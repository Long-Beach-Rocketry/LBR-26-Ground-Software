/**
 * @file LoRa/include/connector/checksum.h
 * @brief Checksum utilities for connector messages
 */

#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace connector {

/**
 * @brief Compute CRC32 checksum of a byte buffer.
 * @param data Pointer to data buffer.
 * @param size Size of buffer in bytes.
 * @return CRC32 checksum as hex string (lowercase, 8 characters).
 */
std::string compute_crc32_hex(const uint8_t* data, size_t size) noexcept;

/**
 * @brief Compute CRC32 checksum of a string.
 * @param data String data.
 * @return CRC32 checksum as hex string (lowercase, 8 characters).
 */
std::string compute_crc32_hex(const std::string& data) noexcept;

/**
 * @brief Validate a CRC32 checksum.
 * @param data Pointer to data buffer.
 * @param size Size of buffer in bytes.
 * @param expected_hex Expected checksum in hex format.
 * @return True if computed checksum matches expected value.
 */
bool validate_crc32(const uint8_t* data, size_t size, const std::string& expected_hex) noexcept;

/**
 * @brief Check if a string is a valid CRC32 hex token.
 * @param checksum_hex Candidate checksum string.
 * @return True if exactly 8 hexadecimal characters.
 */
bool is_valid_crc32_hex(const std::string& checksum_hex) noexcept;

}  // namespace connector
