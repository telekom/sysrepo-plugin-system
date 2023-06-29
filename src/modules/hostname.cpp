#include "hostname.hpp"
#include "srpcpp/ds-check.hpp"

#include <core/common.hpp>

// sethostname() and gethostname()
#include <unistd.h>
#include <stdexcept>

#include <sysrepo.h>

namespace ietf::sys {
/**
 * @brief Get hostname.
 *
 * @return Hostname.
 */
Hostname getHostname()
{
    char hostname[ietf::sys::HOSTNAME_MAX_LEN + 1] = { 0 };

    if (gethostname(hostname, sizeof(hostname)) < 0) {
        throw std::runtime_error("Failed to get hostname.");
    }

    return hostname;
}

/**
 * @brief Set system hostname. Throws a runtime_error if unable to set hostname.
 *
 * @param hostname Hostname.
 */
void setHostname(const Hostname& hostname)
{
    if (auto err = sethostname(hostname.c_str(), hostname.size()); err != 0) {
        throw std::runtime_error("Failed to set hostname.");
    }
}
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

    auto hostname = sys::getHostname();

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

    switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                    case sysrepo::ChangeOperation::Created:
                    case sysrepo::ChangeOperation::Modified:
                        {
                            // modified hostname - get current value and use sethostname()
                            auto value = change.node.asTerm().value();
                            auto hostname = std::get<sys::Hostname>(value);

                            try {
                                sys::setHostname(hostname);
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
srpc::DatastoreValuesCheckStatus HostnameValueChecker::checkValues(sysrepo::Session& session)
{
    srpc::DatastoreValuesCheckStatus status;

    // load hostname from the system and check if the value is the same as in the current session

    return status;
}

/**
 * Hostname module constructor. Allocates each context.
 */
HostnameModule::HostnameModule()
{
    m_operContext = std::make_shared<HostnameOperationalContext>();
    m_changeContext = std::make_shared<HostnameModuleChangesContext>();
    m_rpcContext = std::make_shared<HostnameRpcContext>();
    m_valueChecker = std::make_shared<HostnameValueChecker>();
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
 * Get all system value checkers that this module provides.
 */
std::list<std::shared_ptr<srpc::DatastoreValuesChecker>> HostnameModule::getValueCheckers()
{
    return {
        m_valueChecker,
    };
}

/**
 * Get module name.
 */
constexpr const char* HostnameModule::getName() { return "Hostname"; }
