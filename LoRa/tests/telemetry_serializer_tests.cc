/**
 * @file LoRa/tests/telemetry_serializer_tests.cc
 * @brief Unit Tests for TelemetryMessageSerializer with dynamic field registry
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include <gtest/gtest.h>

#include "telemetry/decoded_telemetry_serializer.h"
#include "telemetry/telemetry_fields.h"

#include <algorithm>

TEST(TelemetrySerializerTests, FieldRegistryContainsFourProtoFields) {
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS.size(), static_cast<size_t>(4));
}

TEST(TelemetrySerializerTests, FieldRegistryContainsCorrectFieldNames) {
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].name, "field_1");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].name, "field_2");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].name, "field_3");
    EXPECT_STREQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].name, "field_4");
}

TEST(TelemetrySerializerTests, FieldRegistryContainsCorrectFieldNumbers) {
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].proto_field_number, 1U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].proto_field_number, 2U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].proto_field_number, 3U);
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].proto_field_number, 4U);
}

TEST(TelemetrySerializerTests, FieldRegistryConvertsFunctionsWork) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 42;
    data.field_2 = 1000;
    data.field_3 = 500;
    data.field_4 = 85;

    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[0].to_string_fn(data), "42");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[1].to_string_fn(data), "1000");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[2].to_string_fn(data), "500");
    EXPECT_EQ(Telemetry::TELEMETRY_PROTO_FIELDS[3].to_string_fn(data), "85");
}

TEST(TelemetrySerializerTests, ToJsonIncludesAllProtoFields) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 2;
    data.field_2 = 16;
    data.field_3 = 500;
    data.field_4 = 87;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string json = serializer.to_json();

    EXPECT_NE(json.find("\"field_1\":\"2\""), std::string::npos);
    EXPECT_NE(json.find("\"field_2\":\"16\""), std::string::npos);
    EXPECT_NE(json.find("\"field_3\":\"500\""), std::string::npos);
    EXPECT_NE(json.find("\"field_4\":\"87\""), std::string::npos);
}

TEST(TelemetrySerializerTests, ToJsonHasValidJsonStructure) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 1;
    data.field_2 = 100;
    data.field_3 = 200;
    data.field_4 = 50;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string json = serializer.to_json();

    EXPECT_EQ(json[0], '{');
    EXPECT_EQ(json[json.length() - 1], '}');
    EXPECT_NE(json.find(":"), std::string::npos);
}

TEST(TelemetrySerializerTests, ToJsonUsesGenericFieldNames) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 1;
    data.field_2 = 100;
    data.field_3 = 200;
    data.field_4 = 50;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string json = serializer.to_json();

    EXPECT_NE(json.find("field_1"), std::string::npos);
    EXPECT_NE(json.find("field_2"), std::string::npos);
    EXPECT_NE(json.find("field_3"), std::string::npos);
    EXPECT_NE(json.find("field_4"), std::string::npos);
}

TEST(TelemetrySerializerTests, ToCsvIncludesAllProtoFields) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 2;
    data.field_2 = 16;
    data.field_3 = 500;
    data.field_4 = 87;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string csv = serializer.to_csv();

    const int comma_count = std::count(csv.begin(), csv.end(), ',');
    EXPECT_EQ(comma_count, 3);

    EXPECT_NE(csv.find("\"2\""), std::string::npos);
    EXPECT_NE(csv.find("\"16\""), std::string::npos);
    EXPECT_NE(csv.find("\"500\""), std::string::npos);
    EXPECT_NE(csv.find("\"87\""), std::string::npos);
}

TEST(TelemetrySerializerTests, ToCsvStartsWithFirstProtoField) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 1;
    data.field_2 = 100;
    data.field_3 = 200;
    data.field_4 = 50;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string csv = serializer.to_csv();

    EXPECT_EQ(csv.substr(0, 3), "\"1\"");
}

TEST(TelemetrySerializerTests, ToCsvHasAllEmptyFieldsQuoted) {
    TelemetryMessage data = TelemetryMessage_init_zero;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string csv = serializer.to_csv();

    EXPECT_NE(csv.find("\"0\""), std::string::npos);
}

TEST(TelemetrySerializerTests, JsonAndCsvIncludeSameProtoFieldValues) {
    TelemetryMessage data = TelemetryMessage_init_zero;
    data.field_1 = 42;
    data.field_2 = 1000;
    data.field_3 = 500;
    data.field_4 = 85;

    Telemetry::TelemetryMessageSerializer serializer(data);
    const std::string json = serializer.to_json();
    const std::string csv = serializer.to_csv();

    EXPECT_NE(json.find("42"), std::string::npos);
    EXPECT_NE(csv.find("42"), std::string::npos);
    EXPECT_NE(json.find("1000"), std::string::npos);
    EXPECT_NE(csv.find("1000"), std::string::npos);
}