#include "hostname.hpp"
#include "core/context.hpp"

#include <core/common.hpp>

// sethostname() and gethostname()
#include <unistd.h>
#include <stdexcept>

#include <sysrepo.h>

namespace ietf::sys {
/**
 * @brief Hostname constructor.
 */
Hostname::Hostname()
    : SdBus<std::string, std::string, bool>(
        "org.freedesktop.hostname1", "/org/freedesktop/hostname1", "org.freedesktop.hostname1", "SetStaticHostname", "Hostname")
{
}

/**
 * @brief Get the system hostname.
 *
 * @return System hostname.
 */
std::string Hostname::getValue(void) { return importFromSdBus(); }

/**
 * @brief Set the systme hostname.
 *
 * @param hostname Hostname to set.
 */
void Hostname::setValue(const std::string& hostname) { exportToSdBus(hostname, false); }
}

namespace ietf::sys::sub::oper {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
HostnameOperGetCb::HostnameOperGetCb(std::shared_ptr<HostnameOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/hostname.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
 *
 * @return Error code.
 *
 */
sr::ErrorCode HostnameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    Hostname hostname_handle;
    const auto hostname = hostname_handle.getValue();

    output->newPath("hostname", hostname);

    return error;
}
}

namespace ietf::sys::sub::change {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
HostnameModuleChangeCb::HostnameModuleChangeCb(std::shared_ptr<HostnameModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/hostname.
 *
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param moduleName The module name used for subscribing.
 * @param subXPath The optional xpath used at the time of subscription.
 * @param event Type of the event that has occured.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * SR_EV_DONE, for example) have the same request ID.
 *
 * @return Error code.
 *
 */
sr::ErrorCode HostnameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    Hostname hostname_handle;

    switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                    case sysrepo::ChangeOperation::Created:
                    case sysrepo::ChangeOperation::Modified:
                        {
                            // modified hostname - get current value and use sethostname()
                            auto value = change.node.asTerm().value();
                            auto hostname = std::get<std::string>(value);

                            try {
                                hostname_handle.setValue(hostname);
                            } catch (const std::runtime_error& err) {
                                SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "%s", err.what());
                                error = sr::ErrorCode::OperationFailed;
                            }
                            break;
                        }
                    case sysrepo::ChangeOperation::Deleted:
                        break;
                    case sysrepo::ChangeOperation::Moved:
                        break;
                }
            }
            break;
        default:
            break;
    }

    return error;
}
}

/**
 * @brief Check for the datastore values on the system.
 *
 * @param session Sysrepo session used for retreiving datastore values.
 *
 * @return Enum describing the output of values comparison.
 */
srpc::DatastoreValuesCheckStatus HostnameValueChecker::checkDatastoreValues(sysrepo::Session& session)
{
    srpc::DatastoreValuesCheckStatus status;
    ietf::sys::Hostname hostname;

    const auto hostname_node = session.getData("/ietf-system:system/hostname");

    if (hostname_node.has_value()) {
        try {
            // load the system hostname
            const auto system_hostname = hostname.getValue();
            // get the session hostname
            const auto session_hostname = std::get<std::string>(hostname_node->asTerm().value());

            if (system_hostname == session_hostname) {
                return srpc::DatastoreValuesCheckStatus::Equal;
            } else {
                return srpc::DatastoreValuesCheckStatus::NonExistant;
            }
        } catch (const std::runtime_error& err) {
            SRPLG_LOG_DBG("hostname-value-checker", "Unable to load system hostname: %s", err.what());
            throw std::runtime_error("Unable to determine hostname system status");
        }
    } else {
        // no hostname node found in the running datastore
        return srpc::DatastoreValuesCheckStatus::NonExistant;
    }

    return status;
}

/**
 * @brief Apply datastore content from the provided session to the system.
 *
 * @param session Session to use for retreiving datastore data.
 */
void HostnameValueApplier::applyDatastoreValues(sysrepo::Session& session) { }

/**
 * Hostname module constructor. Allocates each context.
 */
HostnameModule::HostnameModule(ietf::sys::PluginContext& plugin_ctx)
    : srpc::IModule<ietf::sys::PluginContext>(plugin_ctx)
{
    m_operContext = std::make_shared<HostnameOperationalContext>();
    m_changeContext = std::make_shared<HostnameModuleChangesContext>();
    m_rpcContext = std::make_shared<HostnameRpcContext>();
    this->addValueChecker<HostnameValueChecker>();
    this->addValueApplier<HostnameValueApplier>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> HostnameModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> HostnameModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> HostnameModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> HostnameModule::getOperationalCallbacks()
{
    return {
        srpc::OperationalCallback { "/ietf-system:system/hostname", ietf::sys::sub::oper::HostnameOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> HostnameModule::getModuleChangeCallbacks()
{
    return {
        srpc::ModuleChangeCallback { "/ietf-system:system/hostname", ietf::sys::sub::change::HostnameModuleChangeCb(m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> HostnameModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* HostnameModule::getName() { return "Hostname"; }
