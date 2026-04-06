/**
 * @file sx1262_module.cc
 * @brief Skeleton SX1262 LoRa backend implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/sx1262_module.h"

bool periph::SX1262Module::init() {
    return true;
}

void periph::SX1262Module::transmit(uint8_t * /*buf*/, size_t /*len*/) {
    // TODO: Wire actual SX1262 TX implementation.
}

int periph::SX1262Module::receive(uint8_t * /*buf*/) {
    // TODO: Wire actual SX1262 RX implementation.
    return 0;
}
