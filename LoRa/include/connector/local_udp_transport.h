/**
 * @file local_udp_transport.h
 * @brief Local UDP transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#pragma once

#include <cstdint>
#include <string>

namespace connector {
    /**
     * @brief Role of the local UDP transport endpoint.
     */
    enum class LocalUdpMode {
        Server,
        Client
    };

    /**
     * @brief Minimal datagram-based local UDP transport used by the connector layer.
     *
     * Reads can be performed with a timeout so callers avoid indefinite blocking.
     */
    class LocalUdpTransport {
        public:
            /**
             * @brief Creates a local UDP endpoint descriptor.
             * @param mode Endpoint role (server/client).
             * @param host IPv4 host or localhost.
             * @param port UDP port number.
             */
            LocalUdpTransport(LocalUdpMode mode, std::string host, std::uint16_t port);

            /**
             * @brief Releases socket resources when the object goes out of scope.
             */
            ~LocalUdpTransport();

            /**
             * @brief Opens the UDP socket and binds/connects as needed.
             * @throws std::runtime_error on socket creation, bind, or connect failure.
             */
            void open();

            /**
             * @brief Closes active socket handles.
             */
            void close() noexcept;

            /**
             * @brief Sends one datagram payload.
             * @param payload Datagram bytes to send.
             * @throws std::runtime_error when the transport is not open or send fails.
             */
            void write_datagram(const std::string &payload);

            /**
             * @brief Reads one datagram payload with timeout.
             * @param payload Output datagram payload.
             * @param timeout_ms Maximum wait in milliseconds.
             * @return True when one datagram is read, false when timeout occurs.
             * @throws std::runtime_error when the transport is not open or recv fails.
             */
            bool read_datagram(std::string &payload, std::uint32_t timeout_ms);

        private:
            void ensure_open() const;

            LocalUdpMode _mode;
            std::string _host;
            std::uint16_t _port;
            bool _is_open = false;
            bool _has_peer = false;

#ifdef _WIN32
            using SocketHandle = std::intptr_t;
            SocketHandle _socket = -1;
            std::string _peer_ip;
            std::uint16_t _peer_port = 0;
#else
            using SocketHandle = int;
            SocketHandle _socket = -1;
            std::string _peer_ip;
            std::uint16_t _peer_port = 0;
#endif
    };
}
