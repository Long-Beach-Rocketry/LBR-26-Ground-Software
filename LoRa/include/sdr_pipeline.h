/**
 * @file sdr_pipeline.h
 * @brief Minimal SDR pipeline interface
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_SDR_PIPELINE_H_
#define LORA_INCLUDE_SDR_PIPELINE_H_

#include "cli/config.h"
#include "periph/i_lora_module.h"

class SDRPipeline {
    public:
        /**
         * @brief Creates the SDR pipeline instance from validated runtime config.
         * @param settings Runtime settings produced by CLI and config parsing.
         * @param lora_module Non-null LoRa peripheral abstraction used to receive telemetry bytes.
         */
        explicit SDRPipeline(const cli::RuntimeSettings &settings, periph::ILoRaModule &lora_module);

        /**
         * @brief Starts the SDR processing workflow.
         */
        void run();

    private:
        cli::RuntimeSettings _settings;
        periph::ILoRaModule &_lora_module;
};

    #endif  // LORA_INCLUDE_SDR_PIPELINE_H_
