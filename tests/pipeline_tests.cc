/**
 * @file pipeline_tests.cc
 * @brief Unit tests for SDRPipeline
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "sdr_pipeline.h"
#include "test_common.h"

#include <string>

TEST(PipelineTests, All) {
    {
        cli::RuntimeSettings settings;
        settings.pipeline.verbose = false;
        settings.pipeline.output_path = "silent.bin";

        SDRPipeline pipeline(settings);
        const std::string output = tests::capture_stdout([&pipeline]() { pipeline.run(); });
        EXPECT_TRUE(output.empty());
    }

    {
        cli::RuntimeSettings settings;
        settings.pipeline.verbose = true;
        settings.sdr.device = "rtlsdr";
        settings.sdr.sample_rate_hz = 2048000;
        settings.sdr.center_freq_hz = 433920000;
        settings.sdr.gain_db = 30;
        settings.pipeline.output_path = "output/frame.bin";

        SDRPipeline pipeline(settings);
        const std::string output = tests::capture_stdout([&pipeline]() { pipeline.run(); });
        EXPECT_NE(output.find("Starting SDR pipeline"), std::string::npos);
        EXPECT_NE(output.find("device: rtlsdr"), std::string::npos);
        EXPECT_NE(output.find("output_path: output/frame.bin"), std::string::npos);
    }
}
