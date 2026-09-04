/**
 * @file sx127_module.h
 * @brief Virtual SX127 LoRa module
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_PERIPH_SX127_MODULE_H_
#define LORA_INCLUDE_PERIPH_SX127_MODULE_H_

#include "periph/i_lora_module.h"

#include <deque>
#include <vector>

namespace periph {
    /**
    * @brief Virtual SX127 module used by the current pipeline.
     */
    class SX127Module final : public ILoRaModule {
        public:
            LoRaStatusCode init() override;
            LoRaTransmitResult transmit(std::span<const std::uint8_t> payload) override;
            LoRaReceiveResult receive(std::span<std::uint8_t> buffer,
                                      std::uint32_t timeout_ms) override;

        private:
            bool _initialized = false;
            bool _default_frame_emitted = false;
            std::deque<std::vector<std::uint8_t>> _pending_frames;
    };
}

#endif  // LORA_INCLUDE_PERIPH_SX127_MODULE_H_
