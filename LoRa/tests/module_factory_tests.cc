/**
 * @file module_factory_tests.cc
 * @brief Unit tests for the LoRa module factory
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "periph/lora_module_factory.h"
#include "periph/sx1262_module.h"
#include "periph/sx127_module.h"

#include <stdexcept>
#include <string>
#include <vector>

TEST(LoRaModuleFactoryTests, CreatesRegisteredModules) {
    const auto sx1262 = periph::LoRaModuleFactory::create("sx1262");
    const auto sx127 = periph::LoRaModuleFactory::create("sx127");

    ASSERT_NE(sx1262, nullptr);
    ASSERT_NE(sx127, nullptr);
    EXPECT_NE(dynamic_cast<periph::SX1262Module *>(sx1262.get()), nullptr);
    EXPECT_NE(dynamic_cast<periph::SX127Module *>(sx127.get()), nullptr);
}

TEST(LoRaModuleFactoryTests, ExposesRegisteredNames) {
    EXPECT_TRUE(periph::LoRaModuleFactory::is_supported("sx1262"));
    EXPECT_TRUE(periph::LoRaModuleFactory::is_supported("sx127"));
    EXPECT_FALSE(periph::LoRaModuleFactory::is_supported("unknown"));

    const std::vector<std::string> names = periph::LoRaModuleFactory::supported_modules();
    EXPECT_EQ(names, (std::vector<std::string>{"sx1262", "sx127"}));
}

TEST(LoRaModuleFactoryTests, RejectsUnknownModules) {
    EXPECT_THROW(static_cast<void>(periph::LoRaModuleFactory::create("unknown")),
                 std::invalid_argument);
}
