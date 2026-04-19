/**
 * @file local_zmq_transport.h
 * @brief Optional local ZeroMQ transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstdint>
#include <string>

namespace connector {
    /**
     * @brief Role of the local ZeroMQ endpoint.
     */
    enum class LocalZmqMode {
        Publisher,
        Subscriber
    };

    /**
     * @brief Topic-based local ZeroMQ transport.
     *
     * This class is compiled in all builds. Actual ZeroMQ behavior is enabled only
     * when CMake sets LBR_HAS_ZEROMQ=1.
     */
    class LocalZmqTransport {
        public:
            /**
             * @brief Creates a ZeroMQ endpoint descriptor.
             * @param mode Endpoint role (publisher/subscriber).
             * @param endpoint ZeroMQ endpoint string (e.g. tcp://127.0.0.1:5560).
             * @param topic Topic prefix used by publisher/subscriber filtering.
             */
            LocalZmqTransport(LocalZmqMode mode, std::string endpoint, std::string topic);

            /**
             * @brief Releases underlying resources.
             */
            ~LocalZmqTransport();

            /**
             * @brief Opens the transport endpoint.
             * @throws std::runtime_error on failure or when ZeroMQ is not available.
             */
            void open();

            /**
             * @brief Closes the transport endpoint.
             */
            void close() noexcept;

            /**
             * @brief Sends one topic-framed payload.
             * @param payload Payload bytes.
             * @throws std::runtime_error on failure.
             */
            void send_message(const std::string &payload);

            /**
             * @brief Reads one payload with timeout.
             * @param payload Output payload bytes.
             * @param timeout_ms Maximum wait in milliseconds.
             * @return True if payload is received, false on timeout.
             * @throws std::runtime_error on failure.
             */
            bool receive_message(std::string &payload, std::int32_t timeout_ms);

        private:
            void ensure_open() const;

            LocalZmqMode _mode;
            std::string _endpoint;
            std::string _topic;
            bool _is_open = false;

            struct Impl;
            Impl *_impl = nullptr;
    };
}
