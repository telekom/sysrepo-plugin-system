#include "timezone-name.hpp"

#include <core/common.hpp>

#include <filesystem>

#include <sysrepo.h>

#include "core/sdbus.hpp"

namespace ietf::sys {

/**
 * @brief Default constructor.
 */
TimezoneName::TimezoneName()
    : SdBus<std::string, std::string, bool>(
        "org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTimezone", "Timezone")
{
}

/**
 * @brief Get timezone name value from the system.
 *
 * @return System timezone name.
 */
std::string TimezoneName::getValue(void) { return this->importFromSdBus(); }

/**
 * @brief Set the timezone name on the system.
 *
 * @param timezone_name Timezone to set.
 */
void TimezoneName::setValue(const std::string& timezone_name) { this->exportToSdBus(timezone_name, false); }

}

namespace ietf::sys::sub::oper {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
ClockTimezoneNameOperGetCb::ClockTimezoneNameOperGetCb(std::shared_ptr<TimezoneOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/clock/timezone-name.
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
sr::ErrorCode ClockTimezoneNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    TimezoneName tz_handle;

    auto tz_name = tz_handle.getValue();

    output->newPath("timezone-name", tz_name);

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
ClockTimezoneUtcOffsetOperGetCb::ClockTimezoneUtcOffsetOperGetCb(std::shared_ptr<TimezoneOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/clock/timezone-utc-offset.
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
sr::ErrorCode ClockTimezoneUtcOffsetOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
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
ClockTimezoneNameModuleChangeCb::ClockTimezoneNameModuleChangeCb(std::shared_ptr<TimezoneModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/clock/timezone-name.
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
sr::ErrorCode ClockTimezoneNameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    TimezoneName tz_handle;

    switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                    case sysrepo::ChangeOperation::Created:
                    case sysrepo::ChangeOperation::Modified:
                        {
                            auto value = change.node.asTerm().value();
                            auto timezone_name = std::get<std::string>(value);

                            try {
                                tz_handle.setValue(timezone_name);
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

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
ClockTimezoneUtcOffsetModuleChangeCb::ClockTimezoneUtcOffsetModuleChangeCb(std::shared_ptr<TimezoneModuleChangesContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/clock/timezone-utc-offset.
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
sr::ErrorCode ClockTimezoneUtcOffsetModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
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
srpc::DatastoreValuesCheckStatus TimezoneValueChecker::checkValues(sysrepo::Session& session)
{
    srpc::DatastoreValuesCheckStatus status;

    return status;
}

/**
 * Timezone module constructor. Allocates each context.
 */
TimezoneModule::TimezoneModule()
{
    m_operContext = std::make_shared<TimezoneOperationalContext>();
    m_changeContext = std::make_shared<TimezoneModuleChangesContext>();
    m_rpcContext = std::make_shared<TimezoneRpcContext>();
    m_valueChecker = std::make_shared<TimezoneValueChecker>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> TimezoneModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> TimezoneModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> TimezoneModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> TimezoneModule::getOperationalCallbacks()
{
    return {
        srpc::OperationalCallback { "/ietf-system:system/clock/timezone-name", ietf::sys::sub::oper::ClockTimezoneNameOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> TimezoneModule::getModuleChangeCallbacks()
{
    return {
        srpc::ModuleChangeCallback {
            "/ietf-system:system/clock/timezone-name", ietf::sys::sub::change::ClockTimezoneNameModuleChangeCb(m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> TimezoneModule::getRpcCallbacks() { return {}; }

/**
 * Get all system value checkers that this module provides.
 */
std::list<std::shared_ptr<srpc::DatastoreValuesChecker>> TimezoneModule::getValueCheckers()
{
    return {
        m_valueChecker,
    };
}

/**
 * Get module name.
 */
constexpr const char* TimezoneModule::getName() { return "Timezone"; }
