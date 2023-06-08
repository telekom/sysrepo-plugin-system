#pragma once

#include <core/module.hpp>
#include <core/module-registry.hpp>

class AuthOperationalContext : public IModuleContext { };

class AuthModuleChangesContext : public IModuleContext { };

class AuthRpcContext : public IModuleContext { };

/**
 * @brief Authentication container module.
 * @brief Provides callbacks for user list and each user authorized-key list element.
 */
class AuthModule : public IModule {
public:
    /**
     * Authentication module constructor. Allocates each context.
     */
    AuthModule();

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<RpcCallback> getRpcCallbacks() override;

private:
    std::shared_ptr<AuthOperationalContext> m_operContext;
    std::shared_ptr<AuthModuleChangesContext> m_changeContext;
    std::shared_ptr<AuthRpcContext> m_rpcContext;
};
