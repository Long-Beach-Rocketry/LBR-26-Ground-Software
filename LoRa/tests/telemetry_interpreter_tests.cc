/**
 * @file LoRa/tests/telemetry_interpreter_tests.cc
 * @brief Unit tests for proto-backed TelemetryInterpreter (FALLBACK REMOVED)
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 * @note All fallback (legacy FDCAN) decode tests removed. Only protobuf decoding is tested.
 */

#include <gtest/gtest.h>

#include "telemetry/interpreter.h"
#include "telemetry/test_payloads.h"

#include <string>
#include <vector>

using namespace Telemetry;

// =============================================================================
// Null/Empty Payload Tests
// =============================================================================

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForNullPayload) {
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(nullptr, 16, message, summary, decode_source);
    EXPECT_FALSE(result);
    EXPECT_EQ(decode_source, "error");
    EXPECT_NE(summary.find("null"), std::string::npos);
}

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForEmptyPayload) {
    const auto payload = Telemetry::Test::payload_empty();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_FALSE(result);
    EXPECT_EQ(decode_source, "error");
}

TEST(TelemetryInterpreterTests, DecodeReturnsErrorForPayloadTooLarge) {
    std::vector<uint8_t> oversized(257, 0xFFU);
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(oversized.data(), oversized.size(), message, summary, decode_source);
    EXPECT_FALSE(result);
    EXPECT_EQ(decode_source, "error");
}

// =============================================================================
// Valid Protobuf Payload Decoding
// =============================================================================

TEST(TelemetryInterpreterTests, DecodeSucceedsWithValidPayload) {
    const auto payload = Telemetry::Test::payload_mode_2_altitude_16_velocity_500_battery_87();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_TRUE(result);
    EXPECT_EQ(decode_source, "pb_decode");
    EXPECT_EQ(message.field_1, 2U);
    EXPECT_EQ(message.field_2, 16U);
    EXPECT_EQ(message.field_3, 500U);
    EXPECT_EQ(message.field_4, 87U);
}

TEST(TelemetryInterpreterTests, DecodeAllZerosSuccessfully) {
    const auto payload = Telemetry::Test::payload_all_zeros();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_TRUE(result);
    EXPECT_EQ(decode_source, "pb_decode");
    EXPECT_EQ(message.field_1, 0U);
    EXPECT_EQ(message.field_2, 0U);
    EXPECT_EQ(message.field_3, 0U);
    EXPECT_EQ(message.field_4, 0U);
}

TEST(TelemetryInterpreterTests, DecodeAllMaxSuccessfully) {
    const auto payload = Telemetry::Test::payload_all_max();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_TRUE(result);
    EXPECT_EQ(decode_source, "pb_decode");
    EXPECT_EQ(message.field_1, 0xFFFFFFFFU);
    EXPECT_EQ(message.field_2, 0xFFFFFFFFU);
    EXPECT_EQ(message.field_3, 0xFFFFFFFFU);
    EXPECT_EQ(message.field_4, 0xFFFFFFFFU);
}

TEST(TelemetryInterpreterTests, DecodeArbitraryValuesSuccessfully) {
    const auto payload = Telemetry::Test::payload_arbitrary_values();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_TRUE(result);
    EXPECT_EQ(decode_source, "pb_decode");
    EXPECT_EQ(message.field_1, 42U);
    EXPECT_EQ(message.field_2, 0x1234U);
    EXPECT_EQ(message.field_3, 0x5678U);
    EXPECT_EQ(message.field_4, 150U);
}

// =============================================================================
// Summary/Output Tests
// =============================================================================

TEST(TelemetryInterpreterTests, SummaryIncludesDecodeSourceAndFields) {
    const auto payload = Telemetry::Test::payload_mode_2_altitude_16_velocity_500_battery_87();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    
    EXPECT_NE(summary.find("pb_decode"), std::string::npos);
    EXPECT_NE(summary.find("field_1"), std::string::npos);
    EXPECT_NE(summary.find("2"), std::string::npos);
}

// =============================================================================
// API Availability Tests
// =============================================================================

TEST(TelemetryInterpreterTests, NanopbEnabledReturnsTrue) {
    bool enabled = TelemetryInterpreter::nanopb_enabled();
    EXPECT_TRUE(enabled);
}

TEST(TelemetryInterpreterTests, SimulationMockPayloadIsValid) {
    const auto mock_payload = TelemetryInterpreter::simulation_mock_payload();
    EXPECT_FALSE(mock_payload.empty());
    EXPECT_LE(mock_payload.size(), 256U);
}

TEST(TelemetryInterpreterTests, SimulationMockPayloadDecodes) {
    const auto mock_payload = TelemetryInterpreter::simulation_mock_payload();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(mock_payload.data(), mock_payload.size(), 
                                               message, summary, decode_source);
    // Should not segfault; may succeed or fail, but gracefully
    EXPECT_NE(decode_source, "");
}

// =============================================================================
// Invalid Payload Tests
// =============================================================================

TEST(TelemetryInterpreterTests, DecodeTooShortPayloadFails) {
    const auto payload = Telemetry::Test::payload_too_short();
    TelemetryMessage message = TelemetryMessage_init_zero;
    std::string summary;
    std::string decode_source;

    bool result = TelemetryInterpreter::decode(payload.data(), payload.size(), message, summary, decode_source);
    EXPECT_FALSE(result);
    EXPECT_EQ(decode_source, "error");
}
