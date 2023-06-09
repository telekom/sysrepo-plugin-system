#pragma once

#include <core/module.hpp>

/**
 * Operational context for the DNS module.
 */
class DnsOperationalContext : public IModuleContext { };

/**
 * Module changes context for the DNS module.
 */
class DnsModuleChangesContext : public IModuleContext { };

/**
 * RPC context for the DNS module.
 */
class DnsRpcContext : public IModuleContext { };

/**
 * @brief DNS container module.
 */
class DnsModule : public IModule {
public:
    /**
     * DNS module constructor. Allocates each context.
     */
    DnsModule();

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

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * Dns module destructor.
     */
    ~DnsModule() { }

private:
    std::shared_ptr<DnsOperationalContext> m_operContext;
    std::shared_ptr<DnsModuleChangesContext> m_changeContext;
    std::shared_ptr<DnsRpcContext> m_rpcContext;
};