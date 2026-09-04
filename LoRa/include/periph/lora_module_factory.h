/**
 * @file lora_module_factory.h
 * @brief Factory for creating LoRa module implementations from configuration names
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#ifndef LORA_INCLUDE_PERIPH_LORA_MODULE_FACTORY_H_
#define LORA_INCLUDE_PERIPH_LORA_MODULE_FACTORY_H_

#include "periph/i_lora_module.h"

#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace periph {
    /**
     * @brief Creates LoRa module strategies selected by their configuration name.
     *
     * The factory is the single catalogue of module implementations. Adding a new
     * module requires registering its name and creator in the factory implementation;
     * callers do not need to depend on the concrete module class.
     */
    class LoRaModuleFactory final {
        public:
            /**
             * @brief Creates a module for a configured name.
             * @param module_name Configuration identifier, for example "sx1262".
             * @return Newly allocated module implementation.
             * @throws std::invalid_argument when the name is not registered.
             */
            static std::unique_ptr<ILoRaModule> create(std::string_view module_name);

            /**
             * @brief Checks whether a module name is registered.
             * @param module_name Configuration identifier to check.
             * @return True when the factory can create the requested module.
             */
            static bool is_supported(std::string_view module_name) noexcept;

            /**
             * @brief Returns all registered module names in catalogue order.
             * @return Copy of the names accepted by configuration and CLI parsing.
             */
            static std::vector<std::string> supported_modules();
    };
}

#endif  // LORA_INCLUDE_PERIPH_LORA_MODULE_FACTORY_H_
