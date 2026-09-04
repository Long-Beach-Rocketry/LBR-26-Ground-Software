/**
 * @file socket_handle.h
 * @brief RAII wrapper for native TCP and UDP socket handles
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_CONNECTOR_SOCKET_HANDLE_H_
#define LORA_INCLUDE_CONNECTOR_SOCKET_HANDLE_H_

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <winsock2.h>
#else
    #include <unistd.h>
#endif

namespace connector {
#ifdef _WIN32
    using NativeSocket = SOCKET;
#else
    using NativeSocket = int;
#endif

    /**
     * @brief Owns one native socket and closes it when destroyed.
     *
     * Socket handles are movable but deliberately non-copyable so two transport
     * objects cannot accidentally close the same operating-system resource.
     */
    class SocketHandle final {
        public:
            using NativeType = NativeSocket;

#ifdef _WIN32
            static constexpr NativeType Invalid = INVALID_SOCKET;
#else
            static constexpr NativeType Invalid = -1;
#endif

            SocketHandle() noexcept = default;

            explicit SocketHandle(NativeType handle) noexcept : _handle(handle) {}

            ~SocketHandle() {
                reset();
            }

            SocketHandle(const SocketHandle &) = delete;
            SocketHandle &operator=(const SocketHandle &) = delete;

            SocketHandle(SocketHandle &&other) noexcept : _handle(other.release()) {}

            SocketHandle &operator=(SocketHandle &&other) noexcept {
                if (this != &other) {
                    reset();
                    _handle = other.release();
                }
                return *this;
            }

            bool valid() const noexcept {
                return _handle != Invalid;
            }

            NativeType get() const noexcept {
                return _handle;
            }

            void reset(NativeType handle = Invalid) noexcept {
                if (valid()) {
#ifdef _WIN32
                    ::closesocket(_handle);
#else
                    ::close(_handle);
#endif
                }
                _handle = handle;
            }

            NativeType release() noexcept {
                const NativeType handle = _handle;
                _handle = Invalid;
                return handle;
            }

        private:
            NativeType _handle = Invalid;
    };
}

#endif  // LORA_INCLUDE_CONNECTOR_SOCKET_HANDLE_H_
