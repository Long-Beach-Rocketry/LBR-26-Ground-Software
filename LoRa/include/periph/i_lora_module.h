/**
 * @file i_lora_module.h
 * @brief LoRa transceiver abstraction interface for the telemetry pipeline
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_PERIPH_I_LORA_MODULE_H_
#define LORA_INCLUDE_PERIPH_I_LORA_MODULE_H_

#include <cstddef>
#include <cstdint>
#include <span>

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
        std::size_t bytes_transmitted = 0;
    };

    struct LoRaReceiveResult {
        LoRaStatusCode status = LoRaStatusCode::UnknownError;
        std::size_t bytes_received = 0;
        LoRaSignalMetrics signal;
    };

    /**
     * @brief Strategy interface implemented by all LoRa radio modules.
     *
     * SDRPipeline depends on this abstraction to stay hardware-agnostic while
     * LoRaModuleFactory selects the concrete strategy at runtime.
     */
    class ILoRaModule {
        public:
            virtual ~ILoRaModule() = default;

            /**
             * @brief Initialize the LoRa radio module.
             * @return Initialization status.
             */
            virtual LoRaStatusCode init() = 0;

            /**
             * @brief Transmit a telemetry payload.
             * @param payload Non-owning view of payload bytes.
             * @return Transmit status and bytes written to the radio module.
             */
            virtual LoRaTransmitResult transmit(std::span<const std::uint8_t> payload) = 0;

            /**
             * @brief Receive telemetry bytes from the LoRa module.
             * @param buffer Non-owning view of the destination buffer.
             * @param timeout_ms Maximum wait for a frame in milliseconds.
             * @return Receive status, bytes received, and optional signal metrics.
             */
            virtual LoRaReceiveResult receive(std::span<std::uint8_t> buffer,
                                              std::uint32_t timeout_ms) = 0;
    };
}

#endif  // LORA_INCLUDE_PERIPH_I_LORA_MODULE_H_
