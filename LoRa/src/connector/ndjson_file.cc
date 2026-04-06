/**
 * @file ndjson_file.cc
 * @brief File transport for connector messages using NDJSON
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/ndjson_file.h"

#include <stdexcept>
#include <utility>

connector::NdjsonFileWriter::NdjsonFileWriter(const std::filesystem::path &file_path)
    : _stream(file_path, std::ios::binary | std::ios::app) {
    if (!_stream.is_open())
        throw std::runtime_error("Failed to open NDJSON file for writing: " + file_path.string());
}

void connector::NdjsonFileWriter::write(const ConnectorMessage &message) {
    _stream << message.to_json() << '\n';
    _stream.flush();

    if (!_stream)
        throw std::runtime_error("Failed to write NDJSON message.");
}

connector::NdjsonFileReader::NdjsonFileReader(const std::filesystem::path &file_path)
    : _stream(file_path, std::ios::binary) {
    if (!_stream.is_open())
        throw std::runtime_error("Failed to open NDJSON file for reading: " + file_path.string());
}

bool connector::NdjsonFileReader::read_next(ConnectorMessage &message) {
    while (std::getline(_stream, _line_buffer)) {
        if (_line_buffer.empty())
            continue;

        message = ConnectorMessage::from_json(_line_buffer);
        return true;
    }

    return false;
}
