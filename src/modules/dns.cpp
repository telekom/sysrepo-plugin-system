#include "dns.hpp"

/**
 * DNS module constructor. Allocates each context.
 */
DnsModule::DnsModule()
{
    m_operContext = std::make_shared<DnsOperationalContext>();
    m_changeContext = std::make_shared<DnsModuleChangesContext>();
    m_rpcContext = std::make_shared<DnsRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<IModuleContext> DnsModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<IModuleContext> DnsModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<IModuleContext> DnsModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<OperationalCallback> DnsModule::getOperationalCallbacks() { return {}; }

/**
 * Get all module change callbacks which the module should use.
 */
std::list<ModuleChangeCallback> DnsModule::getModuleChangeCallbacks() { return {}; }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<RpcCallback> DnsModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* DnsModule::getName() { return "DNS"; }