#include "ntp.hpp"

/**
 * @brief Apply datastore content from the provided session to the system.
 *
 * @param session Session to use for retreiving datastore data.
 */
void NtpValuesApplier::applyDatastoreValues(sysrepo::Session& session) { }

/**
 * NTP module constructor. Allocates each context.
 */
NtpModule::NtpModule(ietf::sys::PluginContext& plugin_ctx)
    : srpc::IModule<ietf::sys::PluginContext>(plugin_ctx)
{
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> NtpModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> NtpModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> NtpModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> NtpModule::getOperationalCallbacks() { return {}; }

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> NtpModule::getModuleChangeCallbacks() { return {}; }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> NtpModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* NtpModule::getName() { return "NTP"; }
