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
    struct SdrSettings {
        std::string device = "rtlsdr";
        int sample_rate_hz = 2048000;
        int center_freq_hz = 433920000;
        int gain_db = 30;
    };

    struct PipelineSettings {
        bool verbose = false;
        std::string output_path = "output/frame.bin";
    };

    struct RuntimeSettings {
        SdrSettings sdr;
        PipelineSettings pipeline;
    };

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

            /**
             * @brief Returns parsed runtime settings used by the SDR pipeline.
             * @return Immutable runtime settings structure.
             */
            const RuntimeSettings &settings() const noexcept;

        private:
            /**
             * @brief Parses settings from the YAML file path stored in this config.
             * @param error_message Output parse error details when parsing fails.
             * @return True when config file parsing and validation succeeded.
             */
            bool parse_config_file(std::string &error_message);

            std::string _config_file;
            bool _verbose = false;
            RuntimeSettings _settings;
    };
}
