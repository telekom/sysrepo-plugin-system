#include "timezone-name.hpp"

#include <core/common.hpp>

#include <filesystem>

#include <sysrepo.h>

namespace ietf::sys {
/**
 * @brief Get system timezone name from /etc/localtime.
 *
 * @return Timezone name.
 */
TimezoneName getTimezoneName()
{
    if (std::filesystem::exists(ietf::sys::TIMEZONE_FILE_PATH) && std::filesystem::is_symlink(ietf::sys::TIMEZONE_FILE_PATH)) {
        auto link_path = std::filesystem::read_symlink(ietf::sys::TIMEZONE_FILE_PATH);
        auto dir = std::filesystem::path(ietf::sys::TIMEZONE_DIR_PATH);
        auto rel_path = std::filesystem::relative(link_path, dir);
        return rel_path;
    } else {
        throw std::runtime_error("Failed to get timezone name.");
    }
}

/**
 * @brief Set system timezone name. Throws a runtime_error if unable to set timezone.
 *
 * @param timezoneName Timezone name.
 */
void setTimezoneName(const TimezoneName& timezone_name)
{
    namespace fs = std::filesystem;

    // change timezone-name
    auto tz_dir = fs::path(ietf::sys::TIMEZONE_DIR_PATH);
    auto tz_file = tz_dir / timezone_name;

    // check if the file exists
    auto status = fs::status(tz_file);
    if (!fs::exists(status)) {
        throw std::runtime_error("Timezone file does not exist.");
    }

    // check for /etc/localtime symlink
    auto localtime = fs::path("/etc/localtime");
    if (fs::exists(localtime)) {
        // remove the symlink
        try {
            if (auto err = fs::remove(localtime); err != 0) {
                throw std::runtime_error("Failed to remove /etc/localtime symlink.");
            }
        } catch (fs::filesystem_error& err) {
            throw std::runtime_error("Failed to remove /etc/localtime symlink.");
        }
    }

    // symlink removed; create a new one
    try {
        fs::create_symlink(tz_file, localtime);
    } catch (fs::filesystem_error& err) {
        throw std::runtime_error("Failed to create /etc/localtime symlink.");
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

    auto tz_name = sys::getTimezoneName();

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

    switch (event) {
    case sysrepo::Event::Change:
        for (auto& change : session.getChanges(subXPath->data())) {
            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified: {

                // modified hostname - get current value and use sethostname()
                auto value = change.node.asTerm().value();
                auto timezone_name = std::get<ietf::sys::TimezoneName>(value);

                try {
                    sys::setTimezoneName(timezone_name);
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
 * Timezone module constructor. Allocates each context.
 */
TimezoneModule::TimezoneModule()
{
    m_operContext = std::make_shared<TimezoneOperationalContext>();
    m_changeContext = std::make_shared<TimezoneModuleChangesContext>();
    m_rpcContext = std::make_shared<TimezoneRpcContext>();
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
std::list<OperationalCallback> TimezoneModule::getOperationalCallbacks()
{
    return {
        OperationalCallback { "/ietf-system:system/clock/timezone-name", ietf::sys::sub::oper::ClockTimezoneNameOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<ModuleChangeCallback> TimezoneModule::getModuleChangeCallbacks()
{
    return {
        ModuleChangeCallback { "/ietf-system:system/clock/timezone-name", ietf::sys::sub::change::ClockTimezoneNameModuleChangeCb(m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<RpcCallback> TimezoneModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* TimezoneModule::getName() { return "Timezone"; }