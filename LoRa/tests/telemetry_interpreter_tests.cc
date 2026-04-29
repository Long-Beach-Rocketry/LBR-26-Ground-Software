/**
 * @file telemetry_interpreter_tests.cc
 * @brief Unit tests for TelemetryInterpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "telemetry/interpreter.h"

TEST(TelemetryInterpreterTests, FallbackDecodeAcceptsLegacyFdcanFrame) {
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("decode_source=fallback_fdcan"), std::string::npos);
}

TEST(TelemetryInterpreterTests, DecodeRejectsPayloadThatIsNotNanopbOrLegacyFdcan) {
    const std::uint8_t payload[] = {0x01U, 0x02U, 0x03U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(payload, sizeof(payload));

    EXPECT_FALSE(decoded.decoded);
}

TEST(TelemetryInterpreterTests, SimulationMockPayloadIsDeterministic) {
    const std::vector<std::uint8_t> payload = telemetry::TelemetryInterpreter::simulation_mock_payload();

    ASSERT_EQ(payload.size(), static_cast<std::size_t>(9U));
    EXPECT_EQ(payload[0], 0x08U);
    EXPECT_EQ(payload[1], 0x02U);
}
