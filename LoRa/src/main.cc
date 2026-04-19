/**
 * @file main.cc
 * @brief Entry point for SDR RF-to-Binary pipeline
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 *
 * Project: Long Beach Rocketry - Ground Station SDR
 *
 * Description:
 *   Initializes the RF-to-binary processing pipeline.
 *   Sets up IQ source, processing threads, and metrics.
 *   Responsible for clean startup and shutdown.
 *
 * Notes:
 *   - No DSP logic should be implemented directly in this file.
 *   - All processing must be delegated to modules.
*/

#include "cli/config.h"
#include "periph/sx1262_module.h"
#include "periph/sx127_module.h"
#include "sdr_pipeline.h"

#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>

namespace {
    std::unique_ptr<periph::ILoRaModule> create_lora_module(const cli::LoRaSettings &settings) {
        if (settings.mode == "hardware") {
            throw std::runtime_error(
            "Hardware LoRa mode is not implemented yet; use lora.mode=virtual.");
        }

        const std::string &module_name = settings.module;
        if (module_name == "sx127")
            return std::make_unique<periph::SX127Module>();

        return std::make_unique<periph::SX1262Module>();
    }
}

/**
 * @brief Main entry point for the SDR RF-to-binary processing pipeline
 *
 * @param argc Number of command-line arguments
 * @param argv Array of command-line argument strings
 *
 * @return Exit status code (0 for success, non-zero for errors)
 *
 * This function initializes the configuration by parsing command-line arguments,
 * sets up the SDR processing pipeline, and starts the processing loop.
 * It ensures that all resources are properly released on shutdown.
 */
int main(int argc, char * const argv[]) {
    cli::Config config;

    const cli::Args args = cli::Args::from_main(argc, argv);

    switch (config.parse(args)) {
        case cli::ParseStatus::ExitSuccess:
            return 0;
        case cli::ParseStatus::ExitFailure:
            return 1;
        case cli::ParseStatus::Ok:
            break;
    }

    try {
        std::unique_ptr<periph::ILoRaModule> lora_module =
            create_lora_module(config.settings().lora);
        SDRPipeline pipeline(config.settings(), *lora_module);
        pipeline.run();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
