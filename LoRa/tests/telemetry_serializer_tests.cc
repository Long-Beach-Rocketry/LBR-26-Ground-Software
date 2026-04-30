/**
 * @file LoRa/tests/telemetry_serializer_tests.cc
 * @brief Unit Tests for TelemetrySerializer with dynamic field registry
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "telemetry/decoded_telemetry_serializer.h"
#include "telemetry/telemetry_fields.h"

#include <algorithm>

// ============================================================================
// FIELD REGISTRY TESTS
// ============================================================================

TEST(TelemetrySerializerTests, FieldRegistryContainsFourProtoFields) {
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS.size(), static_cast<size_t>(4));
}

TEST(TelemetrySerializerTests, FieldRegistryContainsCorrectFieldNames) {
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].name, "mode");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].name, "altitude_m");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].name, "velocity_cms");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].name, "battery_percent");
}

TEST(TelemetrySerializerTests, FieldRegistryContainsCorrectFieldNumbers) {
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].proto_field_number, 1U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].proto_field_number, 2U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].proto_field_number, 3U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].proto_field_number, 4U);
}

TEST(TelemetrySerializerTests, FieldRegistryConvertsFunctionsWork) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 42;
    data.altitude_m = 1000;
    data.velocity_cms = 500;
    data.battery_percent = 85;
    data.decode_source = "test";
    data.summary = "test summary";

    // Test each field's to_string_fn
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].to_string_fn(data), "42");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].to_string_fn(data), "1000");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].to_string_fn(data), "500");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].to_string_fn(data), "85");
}

// ============================================================================
// JSON SERIALIZATION TESTS
// ============================================================================

TEST(TelemetrySerializerTests, ToJsonIncludesAllProtoFields) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 2;
    data.altitude_m = 16;
    data.velocity_cms = 500;
    data.battery_percent = 87;
    data.decode_source = "fallback_fdcan";
    data.summary = "mode=2,altitude_m=16,velocity_cms=500,battery_percent=87";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string json = serializer.to_json();

    // Verify all proto fields are present
    EXPECT_NE(json.find("\"mode\":\"2\""), std::string::npos);
    EXPECT_NE(json.find("\"altitude_m\":\"16\""), std::string::npos);
    EXPECT_NE(json.find("\"velocity_cms\":\"500\""), std::string::npos);
    EXPECT_NE(json.find("\"battery_percent\":\"87\""), std::string::npos);

    // Verify metadata fields are present
    EXPECT_NE(json.find("\"decoded\":\"true\""), std::string::npos);
    EXPECT_NE(json.find("\"decode_source\":\"fallback_fdcan\""), std::string::npos);
    EXPECT_NE(json.find("\"summary\""), std::string::npos);
}

TEST(TelemetrySerializerTests, ToJsonHasValidJsonStructure) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 1;
    data.altitude_m = 100;
    data.velocity_cms = 200;
    data.battery_percent = 50;
    data.decode_source = "test";
    data.summary = "test";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string json = serializer.to_json();

    // Basic JSON structure checks
    EXPECT_EQ(json[0], '{');
    EXPECT_EQ(json[json.length() - 1], '}');
    EXPECT_NE(json.find(":"), std::string::npos);
}

TEST(TelemetrySerializerTests, ToJsonEscapesSpecialCharacters) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 1;
    data.altitude_m = 100;
    data.velocity_cms = 200;
    data.battery_percent = 50;
    data.decode_source = "test";
    data.summary = R"(summary with "quotes" and \backslashes\)";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string json = serializer.to_json();

    // Should contain escaped quotes and backslashes
    EXPECT_NE(json.find("\\\""), std::string::npos);
    EXPECT_NE(json.find("\\\\"), std::string::npos);
}

// ============================================================================
// CSV SERIALIZATION TESTS
// ============================================================================

TEST(TelemetrySerializerTests, ToCsvIncludesAllProtoFields) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 2;
    data.altitude_m = 16;
    data.velocity_cms = 500;
    data.battery_percent = 87;
    data.decode_source = "fallback_fdcan";
    data.summary = "test";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string csv = serializer.to_csv();

    // CSV should have 7 fields (decoded + 4 proto + 2 metadata)
    const int comma_count = std::count(csv.begin(), csv.end(), ',');
    EXPECT_EQ(comma_count, 6);  // 7 fields = 6 commas

    // Verify fields are present
    EXPECT_NE(csv.find("\"2\""), std::string::npos);      // mode
    EXPECT_NE(csv.find("\"16\""), std::string::npos);     // altitude_m
    EXPECT_NE(csv.find("\"500\""), std::string::npos);    // velocity_cms
    EXPECT_NE(csv.find("\"87\""), std::string::npos);     // battery_percent
}

TEST(TelemetrySerializerTests, ToCsvStartsWithDecodedFlag) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 1;
    data.altitude_m = 100;
    data.velocity_cms = 200;
    data.battery_percent = 50;
    data.decode_source = "test";
    data.summary = "test";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string csv = serializer.to_csv();

    // First field should be "1" (decoded=true) wrapped in quotes
    EXPECT_EQ(csv.substr(0, 3), "\"1\"");
}

TEST(TelemetrySerializerTests, ToCsvHasAllEmptyFieldsQuoted) {
    Telemetry::DecodedTelemetry data;
    data.decoded = false;
    data.mode = 0;
    data.altitude_m = 0;
    data.velocity_cms = 0;
    data.battery_percent = 0;
    data.decode_source = "";
    data.summary = "";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string csv = serializer.to_csv();

    // All fields should be quoted even if empty
    EXPECT_NE(csv.find("\"0\""), std::string::npos);
    EXPECT_NE(csv.find("\"\""), std::string::npos);  // empty quoted fields
}

// ============================================================================
// CONSISTENCY TESTS
// ============================================================================

TEST(TelemetrySerializerTests, JsonAndCsvIncludeSameProtoFieldValues) {
    Telemetry::DecodedTelemetry data;
    data.decoded = true;
    data.mode = 42;
    data.altitude_m = 1000;
    data.velocity_cms = 500;
    data.battery_percent = 85;
    data.decode_source = "test";
    data.summary = "test";

    Telemetry::DecodedTelemetrySerializer serializer(data);
    const std::string json = serializer.to_json();
    const std::string csv = serializer.to_csv();

    // Both should contain the same field values
    EXPECT_NE(json.find("42"), std::string::npos);
    EXPECT_NE(csv.find("42"), std::string::npos);
    EXPECT_NE(json.find("1000"), std::string::npos);
    EXPECT_NE(csv.find("1000"), std::string::npos);
}
