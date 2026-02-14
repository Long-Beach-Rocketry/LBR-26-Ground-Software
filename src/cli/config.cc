/**
 * @file config.cc
 * @brief Command-line parsing and runtime configuration implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "cli/config.h"

#include <iostream>

cli::ParseStatus cli::Config::parse(const Args &args) {
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
    return _verbose;
}
