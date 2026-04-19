/**
 * @file sx1262_module.h
 * @brief Skeleton SX1262 LoRa backend
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include "periph/i_lora_module.h"

#include <deque>
#include <vector>

namespace periph {
    /**
     * @brief Virtual SX1262 backend used by the current pipeline.
     */
    class SX1262Module final : public ILoRaModule {
        public:
            LoRaStatusCode init() override;
            LoRaTransmitResult transmit(const uint8_t *buf, size_t len) override;
            LoRaReceiveResult receive(uint8_t *buf, size_t max_len, uint32_t timeout_ms) override;

        private:
            bool _initialized = false;
            bool _default_frame_emitted = false;
            std::deque<std::vector<std::uint8_t>> _pending_frames;
    };
}
