/**
 * @file local_zmq_transport.cc
 * @brief Optional local ZeroMQ transport for connector messages
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "connector/local_zmq_transport.h"

#include <stdexcept>
#include <utility>

#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
    #include <zmq.h>
#endif

namespace connector {
    struct LocalZmqTransport::Impl {
#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
        void *context = nullptr;
        void *socket = nullptr;
#endif
    };
}

connector::LocalZmqTransport::LocalZmqTransport(LocalZmqMode mode,
                                                std::string endpoint,
                                                std::string topic)
    : _mode(mode),
      _endpoint(std::move(endpoint)),
      _topic(std::move(topic)),
      _impl(new Impl()) {}

connector::LocalZmqTransport::~LocalZmqTransport() {
    close();
    delete _impl;
    _impl = nullptr;
}

void connector::LocalZmqTransport::open() {
    if (_is_open)
        return;

#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
    _impl->context = ::zmq_ctx_new();
    if (_impl->context == nullptr)
        throw std::runtime_error("Failed to create ZeroMQ context.");

    const int socket_type = _mode == LocalZmqMode::Publisher ? ZMQ_PUB : ZMQ_SUB;
    _impl->socket = ::zmq_socket(_impl->context, socket_type);
    if (_impl->socket == nullptr) {
        close();
        throw std::runtime_error("Failed to create ZeroMQ socket.");
    }

    if (_mode == LocalZmqMode::Publisher) {
        if (::zmq_bind(_impl->socket, _endpoint.c_str()) != 0) {
            close();
            throw std::runtime_error("Failed to bind ZeroMQ publisher socket.");
        }
    } else {
        if (::zmq_connect(_impl->socket, _endpoint.c_str()) != 0) {
            close();
            throw std::runtime_error("Failed to connect ZeroMQ subscriber socket.");
        }
        if (::zmq_setsockopt(_impl->socket,
                             ZMQ_SUBSCRIBE,
                             _topic.data(),
                             static_cast<int>(_topic.size())) != 0) {
            close();
            throw std::runtime_error("Failed to subscribe ZeroMQ topic.");
        }
    }

    _is_open = true;
#else
    (void)_endpoint;
    (void)_topic;
    throw std::runtime_error("ZeroMQ support is not enabled in this build.");
#endif
}

void connector::LocalZmqTransport::close() noexcept {
#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
    if (_impl != nullptr && _impl->socket != nullptr) {
        ::zmq_close(_impl->socket);
        _impl->socket = nullptr;
    }

    if (_impl != nullptr && _impl->context != nullptr) {
        ::zmq_ctx_term(_impl->context);
        _impl->context = nullptr;
    }
#endif
    _is_open = false;
}

void connector::LocalZmqTransport::ensure_open() const {
    if (!_is_open)
        throw std::runtime_error("Local ZeroMQ transport is not open.");
}

void connector::LocalZmqTransport::send_message(const std::string &payload) {
    ensure_open();

#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
    if (_mode != LocalZmqMode::Publisher)
        throw std::runtime_error("Only ZeroMQ publisher can send messages.");

    const int sent_topic =
        static_cast<int>(::zmq_send(_impl->socket, _topic.data(), _topic.size(), ZMQ_SNDMORE));
    if (sent_topic < 0)
        throw std::runtime_error("Failed to send ZeroMQ topic frame.");

    const int sent_payload =
        static_cast<int>(::zmq_send(_impl->socket, payload.data(), payload.size(), 0));
    if (sent_payload < 0)
        throw std::runtime_error("Failed to send ZeroMQ payload frame.");
#else
    (void)payload;
    throw std::runtime_error("ZeroMQ support is not enabled in this build.");
#endif
}

bool connector::LocalZmqTransport::receive_message(std::string &payload, std::int32_t timeout_ms) {
    ensure_open();

#if defined(LBR_HAS_ZEROMQ) && LBR_HAS_ZEROMQ
    if (_mode != LocalZmqMode::Subscriber)
        throw std::runtime_error("Only ZeroMQ subscriber can receive messages.");

    if (::zmq_setsockopt(_impl->socket, ZMQ_RCVTIMEO, &timeout_ms, sizeof(timeout_ms)) != 0)
        throw std::runtime_error("Failed to configure ZeroMQ receive timeout.");

    zmq_msg_t topic_frame;
    if (::zmq_msg_init(&topic_frame) != 0)
        throw std::runtime_error("Failed to initialize ZeroMQ topic frame.");

    const int topic_result = ::zmq_msg_recv(&topic_frame, _impl->socket, 0);
    if (topic_result < 0) {
        ::zmq_msg_close(&topic_frame);
        return false;
    }

    ::zmq_msg_close(&topic_frame);

    zmq_msg_t payload_frame;
    if (::zmq_msg_init(&payload_frame) != 0)
        throw std::runtime_error("Failed to initialize ZeroMQ payload frame.");

    const int payload_result = ::zmq_msg_recv(&payload_frame, _impl->socket, 0);
    if (payload_result < 0) {
        ::zmq_msg_close(&payload_frame);
        return false;
    }

    payload.assign(static_cast<const char *>(::zmq_msg_data(&payload_frame)),
                   static_cast<std::size_t>(::zmq_msg_size(&payload_frame)));
    ::zmq_msg_close(&payload_frame);
    return true;
#else
    (void)payload;
    (void)timeout_ms;
    throw std::runtime_error("ZeroMQ support is not enabled in this build.");
#endif
}
