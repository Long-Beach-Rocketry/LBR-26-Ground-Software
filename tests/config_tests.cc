/**
 * @file config_tests.cc
 * @brief Unit tests for cli::Config
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "cli/config.h"
#include "test_common.h"

#include <string>
#include <string_view>

namespace {
    cli::ParseStatus parse_silent(cli::Config &config, const cli::Args &args) {
        cli::ParseStatus status = cli::ParseStatus::ExitFailure;
        tests::capture_stdout([&]() {
            tests::capture_stderr([&]() { status = config.parse(args); });
        });
        return status;
    }
}

TEST(ConfigTests, All) {
    {
        tests::ArgvBuilder builder({"app.exe", "--help"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitSuccess);
    }

    {
        tests::ArgvBuilder builder({"app.exe", "--bad-option"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ArgvBuilder builder({"app.exe", "-c"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ArgvBuilder builder({"app.exe", "-v"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::Ok);
        EXPECT_TRUE(config.verbose());
    }

    {
        tests::ScopedTempFile file(
            "sdr:\n"
            "  device: \"hackrf\"\n"
            "  sample_rate_hz: 1000000\n"
            "  center_freq_hz: 915000000\n"
            "  gain_db: 12\n"
            "pipeline:\n"
            "  verbose: false\n"
            "  output_path: \"telemetry.bin\"\n");

        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::Ok);
        EXPECT_EQ(config.config_file(), file.path().string());
        EXPECT_EQ(config.settings().sdr.device, std::string("hackrf"));
        EXPECT_EQ(config.settings().sdr.sample_rate_hz, 1000000);
        EXPECT_EQ(config.settings().sdr.center_freq_hz, 915000000);
        EXPECT_EQ(config.settings().sdr.gain_db, 12);
        EXPECT_EQ(config.settings().pipeline.output_path, std::string("telemetry.bin"));
        EXPECT_FALSE(config.settings().pipeline.verbose);
    }

    {
        tests::ScopedTempFile file("pipeline:\n  output_path: \"out.bin\"\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::Ok);
        EXPECT_EQ(config.settings().sdr.device, std::string("rtlsdr"));
        EXPECT_EQ(config.settings().sdr.sample_rate_hz, 2048000);
        EXPECT_EQ(config.settings().pipeline.output_path, std::string("out.bin"));
    }

    {
        tests::ScopedTempFile file("pipeline:\n  verbose: false\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string(), "-v"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::Ok);
        EXPECT_TRUE(config.verbose());
    }

    {
        tests::ArgvBuilder builder({"app.exe", "-c", "missing_file.yaml"});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ScopedTempFile file("- item1\n- item2\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ScopedTempFile file("sdr:\n  sample_rate_hz: \"fast\"\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ScopedTempFile file("sdr:\n  sample_rate_hz: -1\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }

    {
        tests::ScopedTempFile file("pipeline:\n  output_path: \"\"\n");
        tests::ArgvBuilder builder({"app.exe", "-c", file.path().string()});
        cli::Config config;
        EXPECT_EQ(parse_silent(config, builder.build()), cli::ParseStatus::ExitFailure);
    }
}
