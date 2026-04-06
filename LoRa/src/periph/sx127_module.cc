/**
 * @file sx127_module.cc
 * @brief Skeleton SX127 LoRa backend implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/sx127_module.h"

bool periph::SX127Module::init() {
    return true;
}

void periph::SX127Module::transmit(uint8_t * /*buf*/, size_t /*len*/) {
    // TODO: Wire actual SX127 TX implementation.
}

int periph::SX127Module::receive(uint8_t * /*buf*/) {
    // TODO: Wire actual SX127 RX implementation.
    return 0;
}
