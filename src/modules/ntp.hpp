#include "core/context.hpp"
#include "srpcpp/datastore.hpp"
#include <srpcpp/module.hpp>
#include <srpcpp/module-registry.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include <string>
#include <list>
#include <optional>

namespace ietf::sys::ntp {

}

namespace ietf::sys::ntp::change {

}

namespace ietf::sys::ntp::oper {

}

/**
 * Operational context for the NTP module.
 */
class NtpOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the NTP module.
 */
class NtpModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the NTP module.
 */
class NtpRpcContext : public srpc::IModuleContext { };

/**
 * @brief Applier used to apply /ietf-system:system/ntp values from the datastore to the system.
 */
class NtpValuesApplier : public srpc::IDatastoreApplier {
    /**
     * @brief Apply datastore content from the provided session to the system.
     *
     * @param session Session to use for retreiving datastore data.
     */
    virtual void applyDatastoreValues(sysrepo::Session& session) override;

    /**
     * @brief Get the paths which the checker/applier is assigned for.
     *
     * @return Assigned paths.
     */
    virtual std::list<std::string> getPaths() override
    {
        return {
            "/ietf-system:system/ntp",
        };
    }
};
/**
 * @brief NTP container module.
 */
class NtpModule : public srpc::IModule<ietf::sys::PluginContext> {
public:
    /**
     * NTP module constructor. Allocates each context.
     */
    NtpModule(ietf::sys::PluginContext& plugin_ctx);

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<srpc::OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<srpc::ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<srpc::RpcCallback> getRpcCallbacks() override;

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * Auth module destructor.
     */
    ~NtpModule() { }

private:
    std::shared_ptr<NtpOperationalContext> m_operContext;
    std::shared_ptr<NtpModuleChangesContext> m_changeContext;
    std::shared_ptr<NtpRpcContext> m_rpcContext;
};
