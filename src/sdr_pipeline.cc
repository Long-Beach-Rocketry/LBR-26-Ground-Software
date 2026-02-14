/**
 * @file sdr_pipeline.cc
 * @brief Minimal SDR pipeline implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "sdr_pipeline.h"

#include <iostream>

SDRPipeline::SDRPipeline(const cli::Config &config) : _config(config) {}

void SDRPipeline::run() {
    if (_config.verbose()) {
        std::cout << "Starting SDR pipeline";
        if (!_config.config_file().empty())
            std::cout << " with config: " << _config.config_file();

        std::cout << '\n';
    }
}
