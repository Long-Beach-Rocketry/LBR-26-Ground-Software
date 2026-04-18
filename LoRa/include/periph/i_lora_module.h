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
    enum class LoRaStatusCode {
        Ok,
        Timeout,
        InvalidArgument,
        NotInitialized,
        IoError,
        Unsupported,
        UnknownError,
    };

    struct LoRaSignalMetrics {
        bool has_signal_metrics = false;
        int rssi_dbm = 0;
        float snr_db = 0.0F;
    };

    struct LoRaTransmitResult {
        LoRaStatusCode status = LoRaStatusCode::UnknownError;
        size_t bytes_transmitted = 0;
    };

    struct LoRaReceiveResult {
        LoRaStatusCode status = LoRaStatusCode::UnknownError;
        size_t bytes_received = 0;
        LoRaSignalMetrics signal;
    };

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
             * @return Initialization status.
             */
            virtual LoRaStatusCode init() = 0;

            /**
             * @brief Transmit a telemetry payload.
             * @param buf Pointer to payload bytes.
             * @param len Number of bytes to send.
             * @return Transmit status and bytes written to radio backend.
             */
            virtual LoRaTransmitResult transmit(const uint8_t *buf, size_t len) = 0;

            /**
             * @brief Receive telemetry bytes from the LoRa backend.
             * @param buf Destination buffer.
             * @param max_len Capacity of destination buffer.
             * @param timeout_ms Maximum wait for a frame in milliseconds.
             * @return Receive status, bytes received, and optional signal metrics.
             */
            virtual LoRaReceiveResult receive(uint8_t *buf, size_t max_len, uint32_t timeout_ms) = 0;
    };
}
