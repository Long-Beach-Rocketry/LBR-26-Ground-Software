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
            LoRaStatusCode init() override;
            LoRaTransmitResult transmit(const uint8_t *buf, size_t len) override;
            LoRaReceiveResult receive(uint8_t *buf, size_t max_len, uint32_t timeout_ms) override;

        private:
            bool _initialized = false;
    };
}
