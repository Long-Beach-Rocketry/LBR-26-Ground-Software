/**
 * @file sx1262_module.h
 * @brief Skeleton SX1262 LoRa backend
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "periph/i_lora_module.h"

namespace periph {
    /**
     * @brief SX1262 placeholder implementation used by the current pipeline.
     */
    class SX1262Module final : public ILoRaModule {
        public:
            bool init() override;
            void transmit(uint8_t *buf, size_t len) override;
            int receive(uint8_t *buf) override;
    };
}
