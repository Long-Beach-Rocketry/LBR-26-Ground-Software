/**
 * @file sx1262_module.cc
 * @brief Virtual SX1262 LoRa module implementation
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/sx1262_module.h"

#include <algorithm>
#include <array>

namespace {
    constexpr std::array<std::uint8_t, 6> default_frame{{2U, 0x10U, 0x00U, 0xF4U, 0x01U, 87U}};
    const periph::LoRaSignalMetrics default_signal{true, -84, 7.5F};

    periph::LoRaReceiveResult copy_frame(std::span<std::uint8_t> buffer,
                                         const std::vector<std::uint8_t> &frame,
                                         const periph::LoRaSignalMetrics &signal) {
        if (buffer.size() < frame.size())
            return {periph::LoRaStatusCode::InvalidArgument, 0, {}};

        std::copy(frame.begin(), frame.end(), buffer.begin());
        return {periph::LoRaStatusCode::Ok, frame.size(), signal};
    }

    std::vector<std::uint8_t> default_frame_payload() {
        return std::vector<std::uint8_t>(default_frame.begin(), default_frame.end());
    }
}

periph::LoRaStatusCode periph::SX1262Module::init() {
    _initialized = true;
    _default_frame_emitted = false;
    _pending_frames.clear();
    return periph::LoRaStatusCode::Ok;
}

periph::LoRaTransmitResult periph::SX1262Module::transmit(
    std::span<const std::uint8_t> payload) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0};

    _pending_frames.emplace_back(payload.begin(), payload.end());
    return {periph::LoRaStatusCode::Ok, payload.size()};
}

periph::LoRaReceiveResult periph::SX1262Module::receive(
    std::span<std::uint8_t> buffer, std::uint32_t /*timeout_ms*/) {
    if (!_initialized)
        return {periph::LoRaStatusCode::NotInitialized, 0, {}};

    if (!_pending_frames.empty()) {
        std::vector<std::uint8_t> frame = std::move(_pending_frames.front());
        _pending_frames.pop_front();
        return copy_frame(buffer, frame, default_signal);
    }

    if (_default_frame_emitted)
        return {periph::LoRaStatusCode::Timeout, 0, {}};

    _default_frame_emitted = true;
    const auto frame_span = default_frame_payload();
    return copy_frame(buffer, frame_span, default_signal);
}
