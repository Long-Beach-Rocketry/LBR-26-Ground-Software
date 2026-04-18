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

#include <filesystem>
#include <fstream>
#include <string>

namespace {
    class FakeLoRaModule final : public periph::ILoRaModule {
        public:
            periph::LoRaStatusCode init() override {
                initialized = true;
                return periph::LoRaStatusCode::Ok;
            }

            periph::LoRaTransmitResult transmit(const uint8_t *buf, size_t len) override {
                if (buf == nullptr && len > 0)
                    return {periph::LoRaStatusCode::InvalidArgument, 0};

                return {periph::LoRaStatusCode::Ok, len};
            }

            periph::LoRaReceiveResult receive(uint8_t *buf, size_t max_len,
                                              uint32_t /*timeout_ms*/) override {
                if (buf == nullptr)
                    return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

                if (max_len < 6)
                    return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

                buf[0] = 2;
                buf[1] = 0x10;
                buf[2] = 0x00;
                buf[3] = 0xF4;
                buf[4] = 0x01;
                buf[5] = 87;
                return {periph::LoRaStatusCode::Ok, 6, {true, -84, 7.5F}};
            }

            bool initialized = false;
    };

    class FailingLoRaModule final : public periph::ILoRaModule {
        public:
            periph::LoRaStatusCode init() override {
                return periph::LoRaStatusCode::IoError;
            }

            periph::LoRaTransmitResult transmit(const uint8_t * /*buf*/, size_t /*len*/) override {
                return {periph::LoRaStatusCode::Unsupported, 0};
            }

            periph::LoRaReceiveResult receive(uint8_t * /*buf*/, size_t /*max_len*/,
                                              uint32_t /*timeout_ms*/) override {
                return {periph::LoRaStatusCode::Unsupported, 0, {}};
            }
    };
}

TEST(PipelineTests, All) {
    {
        cli::RuntimeSettings settings;
        settings.pipeline.verbose = false;
        settings.pipeline.output_path = "silent.bin";
        FakeLoRaModule module;

        SDRPipeline pipeline(settings, module);
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

        SDRPipeline pipeline(settings, module);
        const std::string output = tests::capture_stdout([&pipeline]() { pipeline.run(); });
        EXPECT_NE(output.find("Starting SDR pipeline"), std::string::npos);
        EXPECT_NE(output.find("device: rtlsdr"), std::string::npos);
        EXPECT_NE(output.find("output_path: output/frame.bin"), std::string::npos);
        EXPECT_NE(output.find("lora_module: sx1262"), std::string::npos);
        EXPECT_NE(output.find("telemetry_status: ok"), std::string::npos);
        EXPECT_NE(output.find("telemetry_bytes_received: 6"), std::string::npos);
        EXPECT_NE(output.find("telemetry_decode: telemetry_v1"), std::string::npos);
    }

    {
        cli::RuntimeSettings settings;
        FailingLoRaModule module;
        SDRPipeline pipeline(settings, module);
        EXPECT_THROW(static_cast<void>(pipeline.run()), std::runtime_error);
    }
}

TEST(PipelineTests, Sx1262SkeletonMethodsAreCallable) {
    periph::SX1262Module module;
    uint8_t buffer[4] = {0, 0, 0, 0};

    EXPECT_EQ(module.init(), periph::LoRaStatusCode::Ok);
    EXPECT_EQ(module.transmit(buffer, sizeof(buffer)).status, periph::LoRaStatusCode::Ok);
    const periph::LoRaReceiveResult receive_result = module.receive(buffer, sizeof(buffer), 50);
    EXPECT_EQ(receive_result.status, periph::LoRaStatusCode::Timeout);
    EXPECT_EQ(receive_result.bytes_received, 0U);
}

TEST(PipelineTests, Sx127SkeletonMethodsAreCallable) {
    periph::SX127Module module;
    uint8_t buffer[4] = {0, 0, 0, 0};

    EXPECT_EQ(module.init(), periph::LoRaStatusCode::Ok);
    EXPECT_EQ(module.transmit(buffer, sizeof(buffer)).status, periph::LoRaStatusCode::Ok);
    const periph::LoRaReceiveResult receive_result = module.receive(buffer, sizeof(buffer), 50);
    EXPECT_EQ(receive_result.status, periph::LoRaStatusCode::Timeout);
    EXPECT_EQ(receive_result.bytes_received, 0U);
}

TEST(PipelineTests, WritesReceivedPayloadToOutputFile) {
    const std::filesystem::path output_path =
        std::filesystem::temp_directory_path() / "lbr_pipeline_output_test.bin";
    std::error_code ec;
    std::filesystem::remove(output_path, ec);

    cli::RuntimeSettings settings;
    settings.pipeline.verbose = true;
    settings.pipeline.output_path = output_path.string();
    settings.pipeline.interpret_telemetry = false;
    FakeLoRaModule module;

    SDRPipeline pipeline(settings, module);
    pipeline.run();

    std::ifstream in(output_path, std::ios::binary);
    ASSERT_TRUE(in.good());
    std::string bytes((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    ASSERT_EQ(bytes.size(), static_cast<std::size_t>(6));
    EXPECT_EQ(static_cast<unsigned char>(bytes[0]), 2U);
    EXPECT_EQ(static_cast<unsigned char>(bytes[1]), 0x10U);
    EXPECT_EQ(static_cast<unsigned char>(bytes[2]), 0x00U);
    EXPECT_EQ(static_cast<unsigned char>(bytes[3]), 0xF4U);
    EXPECT_EQ(static_cast<unsigned char>(bytes[4]), 0x01U);
    EXPECT_EQ(static_cast<unsigned char>(bytes[5]), 87U);

    std::filesystem::remove(output_path, ec);
}

TEST(PipelineTests, InvalidOutputPathThrowsWhenPayloadMustBePersisted) {
    cli::RuntimeSettings settings;
    settings.pipeline.verbose = true;
    settings.pipeline.output_path = "";
    settings.pipeline.interpret_telemetry = false;
    FakeLoRaModule module;

    SDRPipeline pipeline(settings, module);
    EXPECT_THROW(static_cast<void>(pipeline.run()), std::runtime_error);
}
