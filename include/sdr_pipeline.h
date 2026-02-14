/**
 * @file sdr_pipeline.h
 * @brief Minimal SDR pipeline interface
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "cli/config.h"

class SDRPipeline {
    public:
        /**
         * @brief Creates the SDR pipeline instance from validated runtime config.
         * @param config Application configuration produced by CLI parsing.
         */
        explicit SDRPipeline(const cli::Config &config);

        /**
         * @brief Starts the SDR processing workflow.
         */
        void run();

    private:
        cli::Config _config;
};
