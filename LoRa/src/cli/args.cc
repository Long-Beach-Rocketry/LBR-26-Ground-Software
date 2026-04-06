/**
 * @file args.cc
 * @brief Command-line arguments container implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "cli/args.h"

#include <stdexcept>

cli::Args cli::Args::from_main(int argc, char * const argv[]) {
    Args args;

    args._values.reserve(static_cast<std::size_t>(argc));
    for (int i = 0; i < argc; ++i)
        args._values.emplace_back(argv[i] == nullptr ? "" : argv[i]);

    return args;
}

std::size_t cli::Args::size() const noexcept {
    return _values.size();
}

std::string_view cli::Args::at(std::size_t index) const {
    if (index >= _values.size())
        throw std::out_of_range("Argument index out of range");

    return _values[index];
}

std::string_view cli::Args::program_name() const noexcept {
    if (_values.empty())
        return "lbr-ground";

    return _values.front();
}
