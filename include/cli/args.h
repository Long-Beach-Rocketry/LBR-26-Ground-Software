/**
 * @file args.h
 * @brief Command-line arguments container
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstddef>
#include <string_view>
#include <vector>

namespace cli {
    class Args {
        public:
            /**
             * @brief Builds an argument view container from the program entrypoint arguments.
             * @param argc Number of arguments provided to main.
             * @param argv Raw argument vector provided to main.
             * @return A lightweight Args object referencing input arguments.
             */
            static Args from_main(int argc, char * const argv[]);

            /**
             * @brief Returns the number of stored arguments.
             * @return Argument count available in this container.
             */
            std::size_t size() const noexcept;

            /**
             * @brief Returns the argument at a given index.
             * @param index Zero-based position of the requested argument.
             * @return Read-only view over the selected argument string.
             */
            std::string_view at(std::size_t index) const;

            /**
             * @brief Returns the program name (first argument) when available.
             * @return Program name view or a fallback executable name.
             */
            std::string_view program_name() const noexcept;

        private:
            std::vector<std::string_view> _values;
    };
}
