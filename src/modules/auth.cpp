#include "auth.hpp"

#include <memory>

/**
 * Authentication module constructor. Allocates each context.
 */
AuthModule::AuthModule()
{
    m_operContext = std::make_shared<AuthOperationalContext>();
    m_changeContext = std::make_shared<AuthModuleChangesContext>();
    m_rpcContext = std::make_shared<AuthRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<OperationalCallback> AuthModule::getOperationalCallbacks() { }

/**
 * Get all module change callbacks which the module should use.
 */
std::list<ModuleChangeCallback> AuthModule::getModuleChangeCallbacks() { }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<RpcCallback> AuthModule::getRpcCallbacks() { }
