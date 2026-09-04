/**
 * @file lora_module_factory.cc
 * @brief Factory for creating LoRa module implementations from configuration names
 * @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
 * @note Origin: Long Beach Rocketry
 */

#include "periph/lora_module_factory.h"

#include "periph/sx1262_module.h"
#include "periph/sx127_module.h"

#include <algorithm>
#include <array>
#include <stdexcept>

namespace {
    using ModuleCreator = std::unique_ptr<periph::ILoRaModule> (*)();

    struct ModuleDescriptor {
        std::string_view name;
        ModuleCreator creator;
    };

    std::unique_ptr<periph::ILoRaModule> create_sx1262() {
        return std::make_unique<periph::SX1262Module>();
    }

    std::unique_ptr<periph::ILoRaModule> create_sx127() {
        return std::make_unique<periph::SX127Module>();
    }

    const std::array<ModuleDescriptor, 2> &module_catalog() {
        static const std::array<ModuleDescriptor, 2> catalog{{
            {"sx1262", &create_sx1262},
            {"sx127", &create_sx127},
        }};
        return catalog;
    }

    const ModuleDescriptor *find_module(std::string_view module_name) noexcept {
        const auto &catalog = module_catalog();
        const auto found = std::find_if(catalog.begin(), catalog.end(),
                                        [module_name](const ModuleDescriptor &descriptor) {
                                            return descriptor.name == module_name;
                                        });
        return found == catalog.end() ? nullptr : &*found;
    }
}

std::unique_ptr<periph::ILoRaModule> periph::LoRaModuleFactory::create(
    std::string_view module_name) {
    const ModuleDescriptor *descriptor = find_module(module_name);
    if (descriptor == nullptr)
        throw std::invalid_argument("Unsupported LoRa module: " + std::string(module_name));

    return descriptor->creator();
}

bool periph::LoRaModuleFactory::is_supported(std::string_view module_name) noexcept {
    return find_module(module_name) != nullptr;
}

std::vector<std::string> periph::LoRaModuleFactory::supported_modules() {
    std::vector<std::string> names;
    const auto &catalog = module_catalog();
    names.reserve(catalog.size());
    for (const ModuleDescriptor &descriptor : catalog)
        names.emplace_back(descriptor.name);
    return names;
}
