/**
 * @file sdr_pipeline.cc
 * @brief Minimal SDR pipeline implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "sdr_pipeline.h"

#include <array>
#include <iostream>
#include <stdexcept>

SDRPipeline::SDRPipeline(const cli::RuntimeSettings &settings, periph::ILoRaModule *lora_module)
    : _settings(settings), _lora_module(lora_module) {}

void SDRPipeline::run() {
    if (_lora_module == nullptr)
        throw std::invalid_argument("LoRa module pointer must not be null.");

    if (!_lora_module->init())
        throw std::runtime_error("Failed to initialize LoRa module.");

    if (!_settings.pipeline.verbose)
        return;

    std::array<uint8_t, 256> telemetry_buffer {};
    const int received_len = _lora_module->receive(telemetry_buffer.data());

    std::cout << "Starting SDR pipeline\n"
              << "  device: " << _settings.sdr.device << '\n'
              << "  sample_rate_hz: " << _settings.sdr.sample_rate_hz << '\n'
              << "  center_freq_hz: " << _settings.sdr.center_freq_hz << '\n'
              << "  gain_db: " << _settings.sdr.gain_db << '\n'
              << "  output_path: " << _settings.pipeline.output_path << '\n'
              << "  lora_module: " << _settings.lora.module << '\n'
              << "  telemetry_bytes_received: " << received_len << '\n';
}
