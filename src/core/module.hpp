#pragma once

#include "callbacks.hpp"

#include <list>

/**
 * @brief Context interface. Used for passing context to each callback.
 */
class IModuleContext { };

/**
 * @brief Module interface.
 * @brief Each module should define its own callbacks and its own modules to use as parameters to callback classes.
 */
class IModule {
public:
    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getOperationalContext() = 0;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getModuleChangesContext() = 0;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getRpcContext() = 0;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<OperationalCallback> getOperationalCallbacks() = 0;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<ModuleChangeCallback> getModuleChangeCallbacks() = 0;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<RpcCallback> getRpcCallbacks() = 0;

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() = 0;

    /**
     * Virtual destructor.
     */
    virtual ~IModule() { }
};

using ModuleCreateFn = IModule* (*)();
