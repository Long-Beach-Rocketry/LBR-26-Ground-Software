/**
 * @file pipeline_tests.cc
 * @brief Unit tests for SDRPipeline
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "periph/sx1262_module.h"
#include "periph/sx127_module.h"
#include "sdr_pipeline.h"
#include "test_common.h"

#include <string>

namespace {
    class FakeLoRaModule final : public periph::ILoRaModule {
        public:
            bool init() override {
                initialized = true;
                return true;
            }

            void transmit(uint8_t * /*buf*/, size_t /*len*/) override {}

            int receive(uint8_t *buf) override {
                if (buf == nullptr)
                    return 0;

                buf[0] = 0xAA;
                buf[1] = 0x55;
                return 2;
            }

            bool initialized = false;
    };

    class FailingLoRaModule final : public periph::ILoRaModule {
        public:
            bool init() override {
                return false;
            }

            void transmit(uint8_t * /*buf*/, size_t /*len*/) override {}

            int receive(uint8_t * /*buf*/) override {
                return 0;
            }
    };
}

TEST(PipelineTests, All) {
    {
        cli::RuntimeSettings settings;
        settings.pipeline.verbose = false;
        settings.pipeline.output_path = "silent.bin";
        FakeLoRaModule module;

        SDRPipeline pipeline(settings, &module);
        const std::string output = tests::capture_stdout([&pipeline]() { pipeline.run(); });
        EXPECT_TRUE(output.empty());
        EXPECT_TRUE(module.initialized);
    }

    {
        cli::RuntimeSettings settings;
        settings.pipeline.verbose = true;
        settings.sdr.device = "rtlsdr";
        settings.sdr.sample_rate_hz = 2048000;
        settings.sdr.center_freq_hz = 433920000;
        settings.sdr.gain_db = 30;
        settings.pipeline.output_path = "output/frame.bin";
        settings.lora.module = "sx1262";
        FakeLoRaModule module;

        SDRPipeline pipeline(settings, &module);
        const std::string output = tests::capture_stdout([&pipeline]() { pipeline.run(); });
        EXPECT_NE(output.find("Starting SDR pipeline"), std::string::npos);
        EXPECT_NE(output.find("device: rtlsdr"), std::string::npos);
        EXPECT_NE(output.find("output_path: output/frame.bin"), std::string::npos);
        EXPECT_NE(output.find("lora_module: sx1262"), std::string::npos);
        EXPECT_NE(output.find("telemetry_bytes_received: 2"), std::string::npos);
    }

    {
        cli::RuntimeSettings settings;
        SDRPipeline pipeline(settings, nullptr);
        EXPECT_THROW(static_cast<void>(pipeline.run()), std::invalid_argument);
    }

    {
        cli::RuntimeSettings settings;
        FailingLoRaModule module;
        SDRPipeline pipeline(settings, &module);
        EXPECT_THROW(static_cast<void>(pipeline.run()), std::runtime_error);
    }
}

TEST(PipelineTests, Sx1262SkeletonMethodsAreCallable) {
    periph::SX1262Module module;
    uint8_t buffer[4] = {0, 0, 0, 0};

    EXPECT_TRUE(module.init());
    module.transmit(buffer, sizeof(buffer));
    EXPECT_EQ(module.receive(buffer), 0);
}

TEST(PipelineTests, Sx127SkeletonMethodsAreCallable) {
    periph::SX127Module module;
    uint8_t buffer[4] = {0, 0, 0, 0};

    EXPECT_TRUE(module.init());
    module.transmit(buffer, sizeof(buffer));
    EXPECT_EQ(module.receive(buffer), 0);
}
