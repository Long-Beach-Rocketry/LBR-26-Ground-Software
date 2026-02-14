/**
 * @file config.cc
 * @brief Command-line parsing and runtime configuration implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "cli/config.h"

#include <iostream>
#include <string>

#include <yaml-cpp/yaml.h>

namespace {
    template <typename T>
    bool parse_optional_value(const YAML::Node &node,
                              const char *key,
                              T &output,
                              std::string &error_message) {
        if (!node[key])
            return true;

        try {
            output = node[key].as<T>();
            return true;
        } catch (const YAML::Exception &e) {
            error_message = std::string("Invalid value for key '") + key + "': " + e.what();
            return false;
        }
    }
}

cli::ParseStatus cli::Config::parse(const Args &args) {
    _config_file.clear();
    _verbose = false;
    _settings = RuntimeSettings {};

    for (std::size_t i = 1; i < args.size(); ++i) {
        const auto arg = args.at(i);

        if (arg == "-h" || arg == "--help") {
            usage(args.program_name());
            return ParseStatus::ExitSuccess;
        }

        if (arg == "-v" || arg == "--verbose") {
            _verbose = true;
            continue;
        }

        if (arg == "-c" || arg == "--config") {
            if (i + 1 >= args.size()) {
                std::cerr << "Missing value for option: " << arg << '\n';
                usage(args.program_name());
                return ParseStatus::ExitFailure;
            }

            _config_file = std::string(args.at(++i));
            continue;
        }

        std::cerr << "Unknown option: " << arg << '\n';
        usage(args.program_name());
        return ParseStatus::ExitFailure;
    }

    if (!_config_file.empty()) {
        std::string error_message;
        if (!parse_config_file(error_message)) {
            std::cerr << "Failed to parse config file '" << _config_file << "': " << error_message
                      << '\n';
            return ParseStatus::ExitFailure;
        }
    }

    if (_verbose)
        _settings.pipeline.verbose = true;

    return ParseStatus::Ok;
}

void cli::Config::usage(std::string_view program_name) const {
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -h, --help            Show this help message and exit\n"
              << "  -c, --config <file>   Path to configuration file\n"
              << "  -v, --verbose         Enable verbose output" << std::endl;
}

const std::string &cli::Config::config_file() const noexcept {
    return _config_file;
}

bool cli::Config::verbose() const noexcept {
    return _settings.pipeline.verbose;
}

const cli::RuntimeSettings &cli::Config::settings() const noexcept {
    return _settings;
}

bool cli::Config::parse_config_file(std::string &error_message) {
    try {
        const YAML::Node root = YAML::LoadFile(_config_file);
        if (!root || !root.IsMap()) {
            error_message = "Root node must be a YAML map.";
            return false;
        }

        const YAML::Node sdr_node = root["sdr"];
        if (sdr_node) {
            if (!sdr_node.IsMap()) {
                error_message = "Node 'sdr' must be a map.";
                return false;
            }

            if (!parse_optional_value(sdr_node, "device", _settings.sdr.device, error_message))
                return false;
            if (!parse_optional_value(
                    sdr_node, "sample_rate_hz", _settings.sdr.sample_rate_hz, error_message))
                return false;
            if (!parse_optional_value(
                    sdr_node, "center_freq_hz", _settings.sdr.center_freq_hz, error_message))
                return false;
            if (!parse_optional_value(sdr_node, "gain_db", _settings.sdr.gain_db, error_message))
                return false;
        }

        const YAML::Node pipeline_node = root["pipeline"];
        if (pipeline_node) {
            if (!pipeline_node.IsMap()) {
                error_message = "Node 'pipeline' must be a map.";
                return false;
            }

            if (!parse_optional_value(
                    pipeline_node, "verbose", _settings.pipeline.verbose, error_message))
                return false;
            if (!parse_optional_value(
                    pipeline_node, "output_path", _settings.pipeline.output_path, error_message))
                return false;
        }

        if (_settings.sdr.sample_rate_hz <= 0) {
            error_message = "sdr.sample_rate_hz must be > 0.";
            return false;
        }
        if (_settings.sdr.center_freq_hz <= 0) {
            error_message = "sdr.center_freq_hz must be > 0.";
            return false;
        }
        if (_settings.pipeline.output_path.empty()) {
            error_message = "pipeline.output_path must not be empty.";
            return false;
        }
    } catch (const YAML::BadFile &e) {
        error_message = e.what();
        return false;
    } catch (const YAML::Exception &e) {
        error_message = e.what();
        return false;
    }

    return true;
}
