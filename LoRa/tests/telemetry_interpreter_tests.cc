/**
 * @file telemetry_interpreter_tests.cc
 * @brief Unit tests for TelemetryInterpreter with exhaustive case coverage
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "telemetry/interpreter.h"

#include <array>
#include <cstring>
#include <vector>

// ============================================================================
// PAYLOAD VALIDATION TESTS
// ============================================================================

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForNullPayload) {
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(nullptr, 6);

    EXPECT_FALSE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("missing"), std::string::npos);
}

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForEmptyPayload) {
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(reinterpret_cast<const uint8_t *>(""), 0);

    EXPECT_FALSE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("empty"), std::string::npos);
}

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForPayloadTooLarge) {
    std::vector<uint8_t> oversized(257, 0xFFU);
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(oversized.data(), oversized.size());

    EXPECT_FALSE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("large"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeReturnsErrorForNullPayload) {
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(nullptr, 6);

    EXPECT_FALSE(decoded.decoded);
}

// ============================================================================
// LEGACY FDCAN FALLBACK TESTS (6-BYTE PAYLOAD)
// ============================================================================

TEST(TelemetryInterpreterTests, FallbackDecodeAcceptsStandardFdcanFrame) {
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("decode_source=fallback_fdcan"), std::string::npos);
    EXPECT_NE(decoded.summary.find("mode=2"), std::string::npos);
    EXPECT_NE(decoded.summary.find("altitude_m=16"), std::string::npos);
    EXPECT_NE(decoded.summary.find("velocity_cms=500"), std::string::npos);
    EXPECT_NE(decoded.summary.find("battery_percent=87"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeHandlesMinimalValuesCorrectly) {
    const std::uint8_t payload[] = {0U, 0x00U, 0x00U, 0x00U, 0x00U, 0U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("mode=0"), std::string::npos);
    EXPECT_NE(decoded.summary.find("altitude_m=0"), std::string::npos);
    EXPECT_NE(decoded.summary.find("velocity_cms=0"), std::string::npos);
    EXPECT_NE(decoded.summary.find("battery_percent=0"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeHandlesMaximalValuesCorrectly) {
    const std::uint8_t payload[] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("mode=255"), std::string::npos);
    EXPECT_NE(decoded.summary.find("altitude_m=65535"), std::string::npos);
    EXPECT_NE(decoded.summary.find("velocity_cms=65535"), std::string::npos);
    EXPECT_NE(decoded.summary.find("battery_percent=255"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeHandlesLittleEndianAltitudeVelocity) {
    // altitude=0x0100 (LE) = 256, velocity=0x0200 (LE) = 512
    const std::uint8_t payload[] = {0U, 0x00U, 0x01U, 0x00U, 0x02U, 100U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("altitude_m=256"), std::string::npos);
    EXPECT_NE(decoded.summary.find("velocity_cms=512"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeRejectsPayloadThatIsTooShort) {
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_FALSE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("fdcan"), std::string::npos);
}

TEST(TelemetryInterpreterTests, FallbackDecodeRejectsPayloadThatIsTooLong) {
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U, 0xAAU};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_FALSE(decoded.decoded);
}

// ============================================================================
// PRIMARY DECODE ROUTING TESTS
// ============================================================================

TEST(TelemetryInterpreterTests, DecodePrimarybPathFailsAndFallsBackToFdcan) {
    // 6-byte FDCAN payload - cannot be nanopb, should fallback
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_NE(decoded.summary.find("fallback_fdcan"), std::string::npos);
}

TEST(TelemetryInterpreterTests, DecodeRejectsInvalidPayloadSize) {
    const std::uint8_t payload[] = {0x01U, 0x02U, 0x03U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(payload, sizeof(payload));

    EXPECT_FALSE(decoded.decoded);
}

TEST(TelemetryInterpreterTests, DecodeRejectsPayloadWithoutFdcanOrNanopb) {
    // 4 bytes: not 6 (FDCAN) and not valid nanopb
    const std::uint8_t payload[] = {0xAAU, 0xBBU, 0xCCU, 0xDDU};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(payload, sizeof(payload));

    EXPECT_FALSE(decoded.decoded);
}

// ============================================================================
// CONTRACTUAL TESTS
// ============================================================================

TEST(TelemetryInterpreterTests, PayloadLengthValidationIsStrictlyEnforced) {
    // Test all payload sizes from 0 to 10 bytes
    for (std::size_t len = 0; len <= 10; ++len) {
        std::vector<uint8_t> payload(len, 0x01U);
        const telemetry::DecodedTelemetry decoded =
            telemetry::TelemetryInterpreter::decode(payload.data(), len);

        if (len == 6) {
            // Only 6-byte payloads are valid FDCAN
            EXPECT_TRUE(decoded.decoded) << "Size " << len << " should decode via fallback";
        } else {
            // All other sizes should fail
            EXPECT_FALSE(decoded.decoded) << "Size " << len << " should be rejected";
        }
    }
}

TEST(TelemetryInterpreterTests, NanopbEnabledReturnsBoolean) {
    bool enabled = telemetry::TelemetryInterpreter::nanopb_enabled();
    EXPECT_FALSE(enabled) << "Should be false when nanopb headers not available";
}

TEST(TelemetryInterpreterTests, SimulationMockPayloadIsDeterministic) {
    const std::vector<std::uint8_t> payload1 =
        telemetry::TelemetryInterpreter::simulation_mock_payload();
    const std::vector<std::uint8_t> payload2 =
        telemetry::TelemetryInterpreter::simulation_mock_payload();

    EXPECT_EQ(payload1, payload2);
    ASSERT_EQ(payload1.size(), static_cast<std::size_t>(9U));
    EXPECT_EQ(payload1[0], 0x08U);
    EXPECT_EQ(payload1[1], 0x02U);
}

// ============================================================================
// STRUCTURED FIELD POPULATION TESTS
// ============================================================================

TEST(TelemetryInterpreterTests, FallbackDecodePopulatesStructuredFields) {
    const std::uint8_t payload[] = {2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_EQ(decoded.mode, 2U);
    EXPECT_EQ(decoded.altitude_m, 16U);
    EXPECT_EQ(decoded.velocity_cms, 500U);
    EXPECT_EQ(decoded.battery_percent, 87U);
    EXPECT_EQ(decoded.decode_source, "fallback_fdcan");
}

TEST(TelemetryInterpreterTests, FallbackDecodePopulatesMinimalFieldValues) {
    const std::uint8_t payload[] = {0U, 0x00U, 0x00U, 0x00U, 0x00U, 0U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_EQ(decoded.mode, 0U);
    EXPECT_EQ(decoded.altitude_m, 0U);
    EXPECT_EQ(decoded.velocity_cms, 0U);
    EXPECT_EQ(decoded.battery_percent, 0U);
}

TEST(TelemetryInterpreterTests, FallbackDecodePopulatesMaximalFieldValues) {
    const std::uint8_t payload[] = {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::fallback_decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_EQ(decoded.mode, 255U);
    EXPECT_EQ(decoded.altitude_m, 65535U);
    EXPECT_EQ(decoded.velocity_cms, 65535U);
    EXPECT_EQ(decoded.battery_percent, 255U);
}

TEST(TelemetryInterpreterTests, DecodePopulatesFieldsFromFallback) {
    const std::uint8_t payload[] = {42U, 0x34U, 0x12U, 0x78U, 0x56U, 150U};
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(payload, sizeof(payload));

    EXPECT_TRUE(decoded.decoded);
    EXPECT_EQ(decoded.mode, 42U);
    EXPECT_EQ(decoded.altitude_m, 4660U);  // 0x1234 LE
    EXPECT_EQ(decoded.velocity_cms, 22136U);  // 0x5678 LE
    EXPECT_EQ(decoded.battery_percent, 150U);
    EXPECT_EQ(decoded.decode_source, "fallback_fdcan");
}

TEST(TelemetryInterpreterTests, DecodeErrorReturnsZeroFields) {
    const telemetry::DecodedTelemetry decoded =
        telemetry::TelemetryInterpreter::decode(nullptr, 0);

    EXPECT_FALSE(decoded.decoded);
    EXPECT_EQ(decoded.mode, 0U);
    EXPECT_EQ(decoded.altitude_m, 0U);
    EXPECT_EQ(decoded.velocity_cms, 0U);
    EXPECT_EQ(decoded.battery_percent, 0U);
    EXPECT_EQ(decoded.decode_source, "error");
}
