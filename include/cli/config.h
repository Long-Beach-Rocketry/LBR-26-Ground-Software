/**
 * @file config.h
 * @brief Command-line parsing and runtime configuration
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "cli/args.h"

#include <string>
#include <string_view>

namespace cli {
    enum class ParseStatus {
        Ok,
        ExitSuccess,
        ExitFailure
    };

    class Config {
        public:
            /**
             * @brief Parses CLI arguments and updates runtime configuration fields.
             * @param args Parsed argument views to process.
             * @return Parse status indicating success, help exit, or failure.
             */
            ParseStatus parse(const Args &args);

            /**
             * @brief Prints command-line usage text.
             * @param program_name Executable name to show in usage examples.
             */
            void usage(std::string_view program_name) const;

            /**
             * @brief Gets the configured path to the configuration file.
             * @return Config file path string (empty if not provided).
             */
            const std::string &config_file() const noexcept;

            /**
             * @brief Indicates whether verbose mode is enabled.
             * @return True when verbose logging is enabled.
             */
            bool verbose() const noexcept;

        private:
            std::string _config_file;
            bool _verbose = false;
    };
}
