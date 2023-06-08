#pragma once

#include "module.hpp"

#include <typeindex>
#include <map>
#include <memory>
#include <list>
#include <iostream>

using ModuleList = std::list<std::unique_ptr<IModule>>;

/**
 * @brief Module registry. Singleton class for creating and getting modules.
 */
class ModuleRegistry {
public:
    /**
     * Return the singleton instance of the class.
     */
    static ModuleRegistry& getInstance()
    {
        static ModuleRegistry reg;
        return reg;
    }

    /**
     * Register a module.
     */
    template <typename ModuleType> size_t registerModule()
    {
        m_modules.push_back(std::make_unique<ModuleType>());
        return m_modules.size() - 1;
    }

    /**
     * Returns the list of registered modules.
     */
    ModuleList& getRegisteredModules() { return m_modules; }

private:
    ModuleRegistry() = default;
    ~ModuleRegistry() = default;
    ModuleList m_modules;
};
