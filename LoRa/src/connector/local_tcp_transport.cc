/**
 * @file local_tcp_transport.cc
 * @brief Local TCP transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/local_tcp_transport.h"

#include <chrono>
#include <cstring>
#include <stdexcept>
#include <thread>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

namespace {
#ifdef _WIN32
    using NativeSocket = SOCKET;
    constexpr NativeSocket invalid_socket_value = INVALID_SOCKET;

    // Close helper used by both normal and error paths.
    void close_socket(NativeSocket socket_handle) {
        if (socket_handle != invalid_socket_value)
            ::closesocket(socket_handle);
    }

    // Initialize Winsock once per process.
    void ensure_winsock_started() {
        static bool initialized = false;
        if (initialized)
            return;

        WSADATA data {};
        const int result = ::WSAStartup(MAKEWORD(2, 2), &data);
        if (result != 0)
            throw std::runtime_error("WSAStartup failed.");
        initialized = true;
    }
#else
    using NativeSocket = int;
    constexpr NativeSocket invalid_socket_value = -1;

    // POSIX close helper used by both normal and error paths.
    void close_socket(NativeSocket socket_handle) {
        if (socket_handle != invalid_socket_value)
            ::close(socket_handle);
    }
#endif

    // Restrict this transport to IPv4 loopback or explicit IPv4 addresses.
    sockaddr_in make_address(const std::string &host, std::uint16_t port) {
        sockaddr_in address {};
        address.sin_family = AF_INET;
        address.sin_port = htons(port);

        if (host.empty() || host == "127.0.0.1" || host == "localhost") {
            address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
            return address;
        }

        if (::inet_pton(AF_INET, host.c_str(), &address.sin_addr) != 1)
            throw std::runtime_error("Invalid IPv4 host: " + host);

        return address;
    }
}

connector::LocalTcpTransport::LocalTcpTransport(LocalTcpMode mode,
                                                std::string host,
                                                std::uint16_t port)
    : _mode(mode), _host(std::move(host)), _port(port) {}

connector::LocalTcpTransport::~LocalTcpTransport() {
    close();
}

// Open either a listening server socket or a retrying client socket.
void connector::LocalTcpTransport::open() {
    if (_is_open)
        return;

#ifdef _WIN32
    ensure_winsock_started();
#endif

    const sockaddr_in address = make_address(_host, _port);

    if (_mode == LocalTcpMode::Server) {
        _listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (_listen_socket == invalid_socket_value)
            throw std::runtime_error("Failed to create listen socket.");

        int reuse_enabled = 1;
        ::setsockopt(_listen_socket,
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     reinterpret_cast<const char *>(&reuse_enabled),
                     static_cast<int>(sizeof(reuse_enabled)));

        if (::bind(_listen_socket,
                   reinterpret_cast<const sockaddr *>(&address),
                   static_cast<int>(sizeof(address))) != 0) {
            close();
            throw std::runtime_error("Failed to bind local TCP transport.");
        }

        if (::listen(_listen_socket, 1) != 0) {
            close();
            throw std::runtime_error("Failed to listen on local TCP transport.");
        }

        _socket = ::accept(_listen_socket, nullptr, nullptr);
        close_socket(_listen_socket);
        _listen_socket = invalid_socket_value;
        if (_socket == invalid_socket_value) {
            close();
            throw std::runtime_error("Failed to accept local TCP client.");
        }
    } else {
        _socket = ::socket(AF_INET, SOCK_STREAM, 0);
        if (_socket == invalid_socket_value)
            throw std::runtime_error("Failed to create client socket.");

        for (int attempt = 0; attempt < 50; ++attempt) {
            if (::connect(_socket,
                          reinterpret_cast<const sockaddr *>(&address),
                          static_cast<int>(sizeof(address))) == 0) {
                break;
            }
            if (attempt == 49) {
                close();
                throw std::runtime_error("Failed to connect local TCP transport.");
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    _is_open = true;
}

// Close all sockets and reset buffered state; safe to call repeatedly.
void connector::LocalTcpTransport::close() noexcept {
    if (_listen_socket != invalid_socket_value) {
        close_socket(_listen_socket);
        _listen_socket = invalid_socket_value;
    }
    if (_socket != invalid_socket_value) {
        close_socket(_socket);
        _socket = invalid_socket_value;
    }
    _is_open = false;
    _input_buffer.clear();
}

// Guard all I/O operations behind an active connected socket.
void connector::LocalTcpTransport::ensure_open() const {
    if (!_is_open || _socket == invalid_socket_value)
        throw std::runtime_error("Local TCP transport is not open.");
}

// Send the full payload even when the OS accepts only partial writes.
void connector::LocalTcpTransport::send_all(const std::string &data) {
    ensure_open();

    std::size_t total_sent = 0;
    while (total_sent < data.size()) {
#ifdef _WIN32
        const int sent = ::send(_socket,
                                data.data() + total_sent,
                                static_cast<int>(data.size() - total_sent),
                                0);
#else
        const int sent = static_cast<int>(::send(_socket,
                                                 data.data() + total_sent,
                                                 data.size() - total_sent,
                                                 0));
#endif
        if (sent <= 0)
            throw std::runtime_error("Failed to send connector payload.");
        total_sent += static_cast<std::size_t>(sent);
    }
}

// Normalize to line-oriented framing expected by the connector protocol.
void connector::LocalTcpTransport::write_line(const std::string &line) {
    std::string payload = line;
    if (payload.empty() || payload.back() != '\n')
        payload.push_back('\n');
    send_all(payload);
}

// Thin wrapper over recv to isolate platform-specific signatures.
int connector::LocalTcpTransport::recv_some(char *buffer, int buffer_size) {
    ensure_open();
#ifdef _WIN32
    return static_cast<int>(::recv(_socket, buffer, buffer_size, 0));
#else
    return static_cast<int>(::recv(_socket, buffer, buffer_size, 0));
#endif
}

// Return one complete line; preserve remaining bytes for the next read.
bool connector::LocalTcpTransport::read_line(std::string &line) {
    ensure_open();

    for (;;) {
        const std::size_t newline_position = _input_buffer.find('\n');
        if (newline_position != std::string::npos) {
            line = _input_buffer.substr(0, newline_position);
            _input_buffer.erase(0, newline_position + 1);
            return true;
        }

        char buffer[1024] {};
        const int received = recv_some(buffer, static_cast<int>(sizeof(buffer)));
        if (received <= 0) {
            if (_input_buffer.empty())
                return false;

            line = _input_buffer;
            _input_buffer.clear();
            return true;
        }

        _input_buffer.append(buffer, buffer + received);
    }
}
