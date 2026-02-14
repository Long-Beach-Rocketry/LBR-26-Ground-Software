/**
 * @file sdr_pipeline.cc
 * @brief Minimal SDR pipeline implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "sdr_pipeline.h"

#include <iostream>

SDRPipeline::SDRPipeline(const cli::RuntimeSettings &settings) : _settings(settings) {}

void SDRPipeline::run() {
    if (!_settings.pipeline.verbose)
        return;

    std::cout << "Starting SDR pipeline\n"
              << "  device: " << _settings.sdr.device << '\n'
              << "  sample_rate_hz: " << _settings.sdr.sample_rate_hz << '\n'
              << "  center_freq_hz: " << _settings.sdr.center_freq_hz << '\n'
              << "  gain_db: " << _settings.sdr.gain_db << '\n'
              << "  output_path: " << _settings.pipeline.output_path << '\n';
}
