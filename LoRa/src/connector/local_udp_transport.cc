/**
 * @file local_udp_transport.cc
 * @brief Local UDP transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/local_udp_transport.h"

#include <array>
#include <cstring>
#include <stdexcept>
#include <utility>

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
#else
    #include <arpa/inet.h>
    #include <sys/select.h>
    #include <sys/socket.h>
    #include <unistd.h>
#endif

namespace {
#ifdef _WIN32
    using NativeSocket = SOCKET;
    constexpr NativeSocket invalid_socket_value = INVALID_SOCKET;

    void close_socket(NativeSocket socket_handle) {
        if (socket_handle != invalid_socket_value)
            ::closesocket(socket_handle);
    }

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

    void close_socket(NativeSocket socket_handle) {
        if (socket_handle != invalid_socket_value)
            ::close(socket_handle);
    }
#endif

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

connector::LocalUdpTransport::LocalUdpTransport(LocalUdpMode mode,
                                                std::string host,
                                                std::uint16_t port)
    : _mode(mode), _host(std::move(host)), _port(port) {}

connector::LocalUdpTransport::~LocalUdpTransport() {
    close();
}

void connector::LocalUdpTransport::open() {
    if (_is_open)
        return;

#ifdef _WIN32
    ensure_winsock_started();
#endif

    _socket = ::socket(AF_INET, SOCK_DGRAM, 0);
    if (_socket == invalid_socket_value)
        throw std::runtime_error("Failed to create UDP socket.");

    const sockaddr_in address = make_address(_host, _port);

    if (_mode == LocalUdpMode::Server) {
        int reuse_enabled = 1;
        ::setsockopt(_socket,
                     SOL_SOCKET,
                     SO_REUSEADDR,
                     reinterpret_cast<const char *>(&reuse_enabled),
                     static_cast<int>(sizeof(reuse_enabled)));

        if (::bind(_socket,
                   reinterpret_cast<const sockaddr *>(&address),
                   static_cast<int>(sizeof(address))) != 0) {
            close();
            throw std::runtime_error("Failed to bind local UDP transport.");
        }
    } else {
        if (::connect(_socket,
                      reinterpret_cast<const sockaddr *>(&address),
                      static_cast<int>(sizeof(address))) != 0) {
            close();
            throw std::runtime_error("Failed to connect local UDP transport.");
        }
        _has_peer = true;
        _peer_ip = _host.empty() ? "127.0.0.1" : _host;
        _peer_port = _port;
    }

    _is_open = true;
}

void connector::LocalUdpTransport::close() noexcept {
    if (_socket != invalid_socket_value) {
        close_socket(_socket);
        _socket = invalid_socket_value;
    }
    _is_open = false;
    _has_peer = false;
    _peer_ip.clear();
    _peer_port = 0;
}

void connector::LocalUdpTransport::ensure_open() const {
    if (!_is_open || _socket == invalid_socket_value)
        throw std::runtime_error("Local UDP transport is not open.");
}

void connector::LocalUdpTransport::write_datagram(const std::string &payload) {
    ensure_open();

    if (_mode == LocalUdpMode::Client) {
        const int sent = static_cast<int>(::send(_socket, payload.data(), payload.size(), 0));
        if (sent < 0 || static_cast<std::size_t>(sent) != payload.size())
            throw std::runtime_error("Failed to send UDP datagram.");
        return;
    }

    if (!_has_peer)
        throw std::runtime_error("Cannot send UDP datagram before a client peer is known.");

    const sockaddr_in peer = make_address(_peer_ip, _peer_port);
    const int sent = static_cast<int>(::sendto(_socket,
                                               payload.data(),
                                               payload.size(),
                                               0,
                                               reinterpret_cast<const sockaddr *>(&peer),
                                               static_cast<int>(sizeof(peer))));
    if (sent < 0 || static_cast<std::size_t>(sent) != payload.size())
        throw std::runtime_error("Failed to send UDP datagram.");
}

bool connector::LocalUdpTransport::read_datagram(std::string &payload, std::uint32_t timeout_ms) {
    ensure_open();

    fd_set read_set;
    FD_ZERO(&read_set);
    FD_SET(_socket, &read_set);

    timeval timeout {};
    timeout.tv_sec = static_cast<long>(timeout_ms / 1000U);
    timeout.tv_usec = static_cast<long>((timeout_ms % 1000U) * 1000U);

#ifdef _WIN32
    const int select_result = ::select(0, &read_set, nullptr, nullptr, &timeout);
#else
    const int select_result = ::select(_socket + 1, &read_set, nullptr, nullptr, &timeout);
#endif
    if (select_result < 0)
        throw std::runtime_error("Failed waiting on UDP socket.");
    if (select_result == 0)
        return false;

    std::array<char, 65535> buffer {};
    sockaddr_in peer_address {};
#ifdef _WIN32
    int peer_len = static_cast<int>(sizeof(peer_address));
#else
    socklen_t peer_len = static_cast<socklen_t>(sizeof(peer_address));
#endif

    const int received = static_cast<int>(::recvfrom(_socket,
                                                     buffer.data(),
                                                     static_cast<int>(buffer.size()),
                                                     0,
                                                     reinterpret_cast<sockaddr *>(&peer_address),
                                                     &peer_len));
    if (received < 0)
        throw std::runtime_error("Failed to receive UDP datagram.");

    payload.assign(buffer.data(), buffer.data() + received);

    // Learn the last peer in server mode so replies can be sent back.
    if (_mode == LocalUdpMode::Server) {
        char ip_buffer[INET_ADDRSTRLEN] {};
        if (::inet_ntop(AF_INET, &peer_address.sin_addr, ip_buffer, INET_ADDRSTRLEN) != nullptr) {
            _peer_ip = ip_buffer;
            _peer_port = ntohs(peer_address.sin_port);
            _has_peer = true;
        }
    }

    return true;
}
