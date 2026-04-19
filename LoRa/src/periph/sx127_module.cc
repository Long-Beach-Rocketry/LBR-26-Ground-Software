/**
 * @file sx127_module.cc
 * @brief Virtual SX127 LoRa backend implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/sx127_module.h"

#include <array>
#include <algorithm>

namespace {
    constexpr std::array<std::uint8_t, 6> default_frame{{2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U}};
    const periph::LoRaSignalMetrics default_signal{true, -84, 7.5F};

    periph::LoRaReceiveResult copy_frame(uint8_t *buf,
                                         size_t max_len,
                                         const std::vector<std::uint8_t> &frame,
                                         const periph::LoRaSignalMetrics &signal) {
        if (buf == nullptr && max_len > 0)
            return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

        if (max_len < frame.size())
            return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

        std::copy(frame.begin(), frame.end(), buf);
        return {periph::LoRaStatusCode::Ok, frame.size(), signal};
    }

    std::vector<std::uint8_t> default_frame_payload() {
        return {default_frame.begin(), default_frame.end()};
    }
}

periph::LoRaStatusCode periph::SX127Module::init() {
    _initialized = true;
    _default_frame_emitted = false;
    _pending_frames.clear();
    return periph::LoRaStatusCode::Ok;
}

periph::LoRaTransmitResult periph::SX127Module::transmit(const uint8_t *buf, size_t len) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0};

    if (buf == nullptr && len > 0)
        return {periph::LoRaStatusCode::InvalidArgument, 0};

    _pending_frames.emplace_back(buf, buf + len);
    return {periph::LoRaStatusCode::Ok, len};
}

periph::LoRaReceiveResult periph::SX127Module::receive(uint8_t *buf, size_t max_len,
                                                       uint32_t /*timeout_ms*/) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0, {}};

    if (!_pending_frames.empty()) {
        const std::vector<std::uint8_t> frame = _pending_frames.front();
        _pending_frames.pop_front();
        return copy_frame(buf, max_len, frame, default_signal);
    }

    if (_default_frame_emitted)
        return {periph::LoRaStatusCode::Timeout, 0, {}};

    _default_frame_emitted = true;
    const std::vector<std::uint8_t> frame = default_frame_payload();
    return copy_frame(buf, max_len, frame, default_signal);
}
