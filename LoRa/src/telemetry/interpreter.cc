/**
 * @file interpreter.cc
 * @brief Lightweight telemetry payload interpreter
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "telemetry/interpreter.h"

#include <sstream>

telemetry::DecodedTelemetry telemetry::Interpreter::decode(const uint8_t *payload,
                                                          size_t payload_len) {
    if (payload == nullptr)
        return {false, "missing payload buffer"};
 
    // Minimal frame contract used for now:
    // [0]=mode, [1..2]=altitude_m (LE), [3..4]=velocity_cms (LE), [5]=battery_percent
    if (payload_len < 6)
        return {false, "payload too short for telemetry_v1"};

    const uint8_t mode = payload[0];
    const uint16_t altitude_m =
        static_cast<uint16_t>(payload[1]) | (static_cast<uint16_t>(payload[2]) << 8);
    const uint16_t velocity_cms =
        static_cast<uint16_t>(payload[3]) | (static_cast<uint16_t>(payload[4]) << 8);
    const uint8_t battery_percent = payload[5];

    std::ostringstream summary;
    summary << "telemetry_v1"
            << " mode=" << static_cast<int>(mode)
            << " altitude_m=" << altitude_m
            << " velocity_cms=" << velocity_cms
            << " battery_percent=" << static_cast<int>(battery_percent);

    return {true, summary.str()};
}
