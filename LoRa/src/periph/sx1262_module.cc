/**
 * @file sx1262_module.cc
 * @brief Skeleton SX1262 LoRa backend implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/sx1262_module.h"

periph::LoRaStatusCode periph::SX1262Module::init() {
    _initialized = true;
    return periph::LoRaStatusCode::Ok;
}

periph::LoRaTransmitResult periph::SX1262Module::transmit(const uint8_t *buf, size_t len) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0};

    if (buf == nullptr && len > 0)
        return {periph::LoRaStatusCode::InvalidArgument, 0};

    // TODO: Wire actual SX1262 TX implementation.
    return {periph::LoRaStatusCode::Ok, len};
}

periph::LoRaReceiveResult periph::SX1262Module::receive(uint8_t *buf, size_t max_len,
                                                        uint32_t /*timeout_ms*/) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0, {}};

    if (buf == nullptr && max_len > 0)
        return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

    // TODO: Wire actual SX1262 RX implementation.
    return {periph::LoRaStatusCode::Timeout, 0, {}};
}
