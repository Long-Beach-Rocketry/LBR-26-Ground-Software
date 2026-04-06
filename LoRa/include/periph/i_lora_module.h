/**
 * @file i_lora_module.h
 * @brief LoRa transceiver abstraction interface for the telemetry pipeline
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstddef>
#include <cstdint>

namespace periph {
    /**
     * @brief Interface implemented by all LoRa radio backends.
     *
     * SDRPipeline depends on this abstraction to stay hardware-agnostic.
     */
    class ILoRaModule {
        public:
            virtual ~ILoRaModule() = default;

            /**
             * @brief Initialize the LoRa radio backend.
             * @return True if initialization succeeds.
             */
            virtual bool init() = 0;

            /**
             * @brief Transmit a telemetry payload.
             * @param buf Pointer to payload bytes.
             * @param len Number of bytes to send.
             */
            virtual void transmit(uint8_t *buf, size_t len) = 0;

            /**
             * @brief Receive telemetry bytes from the LoRa backend.
             * @param buf Destination buffer.
             * @return Number of bytes received.
             */
            virtual int receive(uint8_t *buf) = 0;
    };
}
