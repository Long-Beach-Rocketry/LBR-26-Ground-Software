/**
 * @file ndjson_file.h
 * @brief File transport for connector messages using NDJSON
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "connector/message.h"

#include <filesystem>
#include <fstream>
#include <string>

namespace connector {
    /**
     * @brief Appends connector messages as NDJSON lines.
     */
    class NdjsonFileWriter {
        public:
            /**
             * @brief Opens an NDJSON output file in append mode.
             * @param file_path Path to the NDJSON file.
             * @throws std::runtime_error when the file cannot be opened.
             */
            explicit NdjsonFileWriter(const std::filesystem::path &file_path);

            /**
             * @brief Writes one connector message as a single NDJSON line.
             * @param message Message to serialize and append.
             * @throws std::runtime_error when serialization or write fails.
             */
            void write(const ConnectorMessage &message);

        private:
            std::ofstream _stream;
    };

    /**
     * @brief Reads connector messages from an NDJSON file stream.
     */
    class NdjsonFileReader {
        public:
            /**
             * @brief Opens an NDJSON input file for sequential reads.
             * @param file_path Path to the NDJSON file.
             * @throws std::runtime_error when the file cannot be opened.
             */
            explicit NdjsonFileReader(const std::filesystem::path &file_path);

            /**
             * @brief Reads the next non-empty NDJSON line and parses it as a connector message.
             * @param message Output parsed message.
             * @return True when a message was parsed, false when EOF is reached.
             * @throws std::runtime_error when a line exists but is malformed JSON.
             */
            bool read_next(ConnectorMessage &message);

        private:
            std::ifstream _stream;
            std::string _line_buffer;
    };
}
