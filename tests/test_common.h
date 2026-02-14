/**
 * @file test_common.h
 * @brief Shared helpers for unit tests
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "cli/args.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace tests {
    class ArgvBuilder {
        public:
            explicit ArgvBuilder(std::initializer_list<std::string> args)
                : _storage(args) {
                _argv.reserve(_storage.size());
                for (std::string &item : _storage)
                    _argv.push_back(item.data());
            }

            cli::Args build() {
                return cli::Args::from_main(static_cast<int>(_argv.size()), _argv.data());
            }

        private:
            std::vector<std::string> _storage;
            std::vector<char *> _argv;
    };

    class ScopedTempFile {
        public:
            explicit ScopedTempFile(const std::string &contents, const std::string &extension = ".yaml") {
                const auto now = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                _path = std::filesystem::temp_directory_path() /
                        ("lbr_test_" + std::to_string(now) + extension);

                std::ofstream out(_path, std::ios::binary);
                out << contents;
            }

            ~ScopedTempFile() {
                std::error_code ec;
                std::filesystem::remove(_path, ec);
            }

            const std::filesystem::path &path() const noexcept {
                return _path;
            }

        private:
            std::filesystem::path _path;
    };

    inline std::string capture_stdout(const std::function<void()> &fn) {
        std::ostringstream buffer;
        auto *old_buf = std::cout.rdbuf(buffer.rdbuf());
        fn();
        std::cout.rdbuf(old_buf);
        return buffer.str();
    }

    inline std::string capture_stderr(const std::function<void()> &fn) {
        std::ostringstream buffer;
        auto *old_buf = std::cerr.rdbuf(buffer.rdbuf());
        fn();
        std::cerr.rdbuf(old_buf);
        return buffer.str();
    }
}
