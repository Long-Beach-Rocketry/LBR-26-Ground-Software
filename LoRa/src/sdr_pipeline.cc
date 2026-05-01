/**
 * @file sdr_pipeline.cc
 * @brief Minimal SDR pipeline implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "sdr_pipeline.h"

#include "telemetry/interpreter.h"

#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace {
    const char *status_to_string(periph::LoRaStatusCode status) {
        switch (status) {
            case periph::LoRaStatusCode::Ok:
                return "ok";
            case periph::LoRaStatusCode::Timeout:
                return "timeout";
            case periph::LoRaStatusCode::InvalidArgument:
                return "invalid_argument";
            case periph::LoRaStatusCode::NotInitialized:
                return "not_initialized";
            case periph::LoRaStatusCode::IoError:
                return "io_error";
            case periph::LoRaStatusCode::Unsupported:
                return "unsupported";
            case periph::LoRaStatusCode::UnknownError:
                return "unknown_error";
        }

        return "unknown_error";
    }

    void persist_payload(const std::string &output_path,
                         const std::uint8_t *payload,
                         std::size_t payload_len) {
        if (payload_len == 0)
            return;

        std::filesystem::path path(output_path);
        const std::filesystem::path parent = path.parent_path();
        if (!parent.empty())
            std::filesystem::create_directories(parent);

        std::ofstream out(path, std::ios::binary | std::ios::trunc);
        if (!out)
            throw std::runtime_error("Failed to open telemetry output file: " + output_path);

        out.write(reinterpret_cast<const char *>(payload), static_cast<std::streamsize>(payload_len));
        if (!out)
            throw std::runtime_error("Failed to write telemetry output file: " + output_path);
    }
}

SDRPipeline::SDRPipeline(const cli::RuntimeSettings &settings, periph::ILoRaModule &lora_module)
    : _settings(settings), _lora_module(lora_module) {}

void SDRPipeline::run() {
    const periph::LoRaStatusCode init_status = _lora_module.init();
    if (init_status != periph::LoRaStatusCode::Ok)
        throw std::runtime_error(std::string("Failed to initialize LoRa module: ") +
                                 status_to_string(init_status));

    if (!_settings.pipeline.verbose)
        return;

    std::array<uint8_t, 256> telemetry_buffer {};
    const periph::LoRaReceiveResult receive_result =
        _lora_module.receive(telemetry_buffer.data(), telemetry_buffer.size(), 50);
    if (receive_result.status != periph::LoRaStatusCode::Ok &&
        receive_result.status != periph::LoRaStatusCode::Timeout)
        throw std::runtime_error(std::string("Failed to receive telemetry: ") +
                                 status_to_string(receive_result.status));

    std::cout << "Starting SDR pipeline\n"
              << "  device: " << _settings.sdr.device << '\n'
              << "  sample_rate_hz: " << _settings.sdr.sample_rate_hz << '\n'
              << "  center_freq_hz: " << _settings.sdr.center_freq_hz << '\n'
              << "  gain_db: " << _settings.sdr.gain_db << '\n'
              << "  output_path: " << _settings.pipeline.output_path << '\n'
              << "  lora_module: " << _settings.lora.module << '\n'
              << "  telemetry_status: " << status_to_string(receive_result.status) << '\n'
              << "  telemetry_bytes_received: " << receive_result.bytes_received << '\n';

    persist_payload(_settings.pipeline.output_path,
                    telemetry_buffer.data(),
                    receive_result.bytes_received);

    if (!_settings.pipeline.interpret_telemetry)
        return;

    const telemetry::DecodedTelemetry decoded =
        telemetry::Interpreter::decode(telemetry_buffer.data(), receive_result.bytes_received);
    if (decoded.decoded)
        std::cout << "  telemetry_decode: " << decoded.summary << '\n';
    else
        std::cout << "  telemetry_decode: unavailable (" << decoded.summary << ")\n";
}
