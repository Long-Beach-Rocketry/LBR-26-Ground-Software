/**
 * @file args_tests.cc
 * @brief Unit tests for cli::Args
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "test_common.h"

#include <stdexcept>
#include <string_view>

TEST(ArgsTests, All) {
    {
        tests::ArgvBuilder builder({"app.exe", "-v", "--config"});
        const cli::Args args = builder.build();

        EXPECT_EQ(args.size(), static_cast<std::size_t>(3));
        EXPECT_EQ(args.program_name(), std::string_view("app.exe"));
        EXPECT_EQ(args.at(1), std::string_view("-v"));
    }

    {
        char *argv[] = {nullptr};
        const cli::Args args = cli::Args::from_main(1, argv);
        EXPECT_EQ(args.program_name(), std::string_view(""));
    }

    {
        char **argv = nullptr;
        const cli::Args args = cli::Args::from_main(0, argv);
        EXPECT_EQ(args.program_name(), std::string_view("lbr-ground"));
    }

    {
        tests::ArgvBuilder builder({"app.exe"});
        const cli::Args args = builder.build();
        EXPECT_THROW(static_cast<void>(args.at(5)), std::out_of_range);
    }
}
