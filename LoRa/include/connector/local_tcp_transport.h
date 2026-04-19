/**
 * @file local_tcp_transport.h
 * @brief Local TCP transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_CONNECTOR_LOCAL_TCP_TRANSPORT_H_
#define LORA_INCLUDE_CONNECTOR_LOCAL_TCP_TRANSPORT_H_

#include <cstdint>
#include <string>

namespace connector {
    /**
     * @brief Role of the local TCP transport endpoint.
     */
    enum class LocalTcpMode {
        Server,
        Client
    };

    /**
     * @brief Minimal line-based local TCP transport used by the connector layer.
     */
    class LocalTcpTransport {
        public:
            /**
             * @brief Creates a local TCP endpoint descriptor.
             * @param mode Endpoint role (server/client).
             * @param host IPv4 host or localhost.
             * @param port TCP port number.
             */
            LocalTcpTransport(LocalTcpMode mode, std::string host, std::uint16_t port);

            /**
             * @brief Releases socket resources when the object goes out of scope.
             */
            ~LocalTcpTransport();

            /**
             * @brief Opens the transport socket and establishes the connection.
             * @throws std::runtime_error on socket creation, bind, listen, accept, or connect failure.
             */
            void open();

            /**
             * @brief Closes active socket handles and resets internal buffers.
             */
            void close() noexcept;

            /**
             * @brief Sends one UTF-8 line payload, appending a newline when needed.
             * @param line Line payload to send.
             * @throws std::runtime_error when the transport is not open or send fails.
             */
            void write_line(const std::string &line);

            /**
             * @brief Reads one line payload from the transport.
             * @param line Output line without trailing newline character.
             * @return True when a line is returned, false when connection closes with no buffered bytes.
             * @throws std::runtime_error when the transport is not open.
             */
            bool read_line(std::string &line);

        private:
            /**
             * @brief Validates that the transport currently owns an active connected socket.
             * @throws std::runtime_error when not connected.
             */
            void ensure_open() const;

            /**
             * @brief Sends an entire string payload by looping until all bytes are transmitted.
             * @param data Raw payload bytes.
             */
            void send_all(const std::string &data);

            /**
             * @brief Receives a chunk of bytes from the connected socket.
             * @param buffer Destination buffer.
             * @param buffer_size Number of bytes available in the destination buffer.
             * @return Number of bytes received, zero on orderly shutdown, or negative on error.
             */
            int recv_some(char *buffer, int buffer_size);

            LocalTcpMode _mode;
            std::string _host;
            std::uint16_t _port;
            bool _is_open = false;
            std::string _input_buffer;

#ifdef _WIN32
            using SocketHandle = std::intptr_t;
            SocketHandle _listen_socket = -1;
            SocketHandle _socket = -1;
#else
            using SocketHandle = int;
            SocketHandle _listen_socket = -1;
            SocketHandle _socket = -1;
#endif
    };
}

#endif  // LORA_INCLUDE_CONNECTOR_LOCAL_TCP_TRANSPORT_H_
