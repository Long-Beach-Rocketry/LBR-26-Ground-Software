/**
 * @file hil_runner.cc
 * @brief Hardware-in-the-loop runner for real LoRa backend validation
 */

#include "periph/i_lora_module.h"
#include "periph/sx1262_module.h"
#include "periph/sx127_module.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>

namespace {
    std::unique_ptr<periph::ILoRaModule> create_lora_module(const std::string &module_name) {
        if (module_name == "sx127")
            return std::make_unique<periph::SX127Module>();
        return std::make_unique<periph::SX1262Module>();
    }

    void usage(const char *program) {
        std::cout << "Usage: " << program << " [--module sx1262|sx127] [--timeout-ms N] [--min-bytes N]\n";
    }
}

int main(int argc, char **argv) {
    std::string module_name = "sx1262";
    std::uint32_t timeout_ms = 3000;
    std::size_t min_bytes = 1;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help") {
            usage(argv[0]);
            return 0;
        }
        if (arg == "--module" && i + 1 < argc) {
            module_name = argv[++i];
            continue;
        }
        if (arg == "--timeout-ms" && i + 1 < argc) {
            timeout_ms = static_cast<std::uint32_t>(std::stoul(argv[++i]));
            continue;
        }
        if (arg == "--min-bytes" && i + 1 < argc) {
            min_bytes = static_cast<std::size_t>(std::stoull(argv[++i]));
            continue;
        }

        std::cerr << "Unknown or incomplete argument: " << arg << '\n';
        usage(argv[0]);
        return 2;
    }

    if (module_name != "sx1262" && module_name != "sx127") {
        std::cerr << "Unsupported module: " << module_name << '\n';
        return 2;
    }

    std::unique_ptr<periph::ILoRaModule> module = create_lora_module(module_name);
    const periph::LoRaStatusCode init_status = module->init();
    if (init_status != periph::LoRaStatusCode::Ok) {
        std::cerr << "HIL init failed for module=" << module_name << '\n';
        return 1;
    }

    std::uint8_t buffer[512] {};
    const auto start = std::chrono::steady_clock::now();

    while (true) {
        const periph::LoRaReceiveResult rx = module->receive(buffer, sizeof(buffer), 100);
        if (rx.status == periph::LoRaStatusCode::Ok && rx.bytes_received >= min_bytes) {
            std::cout << "HIL PASS module=" << module_name
                      << " bytes_received=" << rx.bytes_received
                      << " rssi_dbm=" << rx.signal.rssi_dbm
                      << " snr_db=" << rx.signal.snr_db << '\n';
            return 0;
        }

        const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);
        if (elapsed.count() >= static_cast<long long>(timeout_ms)) {
            std::cerr << "HIL FAIL module=" << module_name
                      << " timeout_ms=" << timeout_ms
                      << " last_status=" << static_cast<int>(rx.status) << '\n';
            return 1;
        }
    }
}
