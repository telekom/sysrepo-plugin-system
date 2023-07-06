#include "system.hpp"
#include "srpcpp/module.hpp"

// Platform information
#include <stdexcept>
#include <memory>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys::state {
/**
 * @brief Get platform information.
 *
 * @return Platform information.
 */
PlatformInfo getPlatformInfo()
{
    struct utsname uname_data = { 0 };

    if (uname(&uname_data) < 0) {
        throw std::runtime_error("Failed to get platform information.");
    }

    return {
        .OsName = uname_data.sysname,
        .OsRelease = uname_data.release,
        .OsVersion = uname_data.version,
        .Machine = uname_data.machine,
    };
}

/**
 * @brief Get clock information.
 *
 * @return Clock information.
 */
ClockInfo getClockInfo()
{
    time_t now = 0;
    struct tm* ts = { 0 };
    struct sysinfo s_info = { 0 };
    time_t uptime_seconds = 0;

    char current_datetime[256];
    char boot_datetime[256];

    now = time(NULL);

    ts = localtime(&now);
    if (ts == NULL) {
        throw std::runtime_error("Failed to get current datetime.");
    }

    strftime(current_datetime, sizeof(current_datetime), "%FT%TZ", ts);

    if (sysinfo(&s_info) != 0) {
        throw std::runtime_error("Failed to get system uptime.");
    }

    uptime_seconds = s_info.uptime;

    time_t diff = now - uptime_seconds;

    ts = localtime(&diff);
    if (ts == NULL) {
        throw std::runtime_error("Failed to get boot datetime.");
    }

    strftime(boot_datetime, sizeof(boot_datetime), "%FT%TZ", ts);

    return {
        .BootDatetime = boot_datetime,
        .CurrentDatetime = current_datetime,
    };
}

/**
 * @brief Set system current datetime. Throws a runtime_error if unable to set datetime.
 *
 * @param datetime Datetime.
 */
void setCurrentDatetime(const std::string& datetime)
{
    struct tm t = { 0 };
    time_t time_to_set = 0;
    struct timespec stime = { 0 };

    if (strptime(datetime.c_str(), "%FT%TZ", &t) == 0) {
        throw std::runtime_error("Failed to parse datetime");
    }

    time_to_set = mktime(&t);
    if (time_to_set == -1) {
        throw std::runtime_error("Failed to convert datetime to time_t");
    }

    stime.tv_sec = time_to_set;

    if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
        throw std::runtime_error("Failed to set system time");
    }
}

/**
 * @brief Restart system.
 */
void restartSystem()
{
    if (auto rc = std::system("reboot"); rc != 0) {
        throw std::runtime_error("Failed to restart system.");
    }
}

/**
 * @brief Shutdown system.
 */
void shutdownSystem()
{
    if (auto rc = std::system("poweroff"); rc != 0) {
        throw std::runtime_error("Failed to shutdown system.");
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
StatePlatformOperGetCb::StatePlatformOperGetCb(std::shared_ptr<SystemOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/platform.
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
sr::ErrorCode StatePlatformOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    namespace sys_state = ietf::sys::state;

    auto platform = output->newPath("platform");

    if (platform) {
        auto platform_info = sys_state::getPlatformInfo();

        platform->newPath("os-name", platform_info.OsName);
        platform->newPath("os-release", platform_info.OsRelease);
        platform->newPath("os-version", platform_info.OsVersion);
        platform->newPath("machine", platform_info.Machine);
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
StateClockOperGetCb::StateClockOperGetCb(std::shared_ptr<SystemOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/clock.
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
sr::ErrorCode StateClockOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    namespace sys_state = ietf::sys::state;
    sr::ErrorCode error = sr::ErrorCode::Ok;

    auto clock = output->newPath("clock");

    if (clock) {
        auto clock_info = sys_state::getClockInfo();

        clock->newPath("current-datetime", clock_info.CurrentDatetime);
        clock->newPath("boot-datetime", clock_info.BootDatetime);
    }

    return error;
}
}

namespace ietf::sys::sub::change {

}

namespace ietf::sys::sub::rpc {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin RPC context.
 *
 */
SetCurrentDatetimeRpcCb::SetCurrentDatetimeRpcCb(std::shared_ptr<SystemRpcContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:set-current-datetime.
 *
 * A callback for RPC/action subscriptions.
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param path Path identifying the RPC/action.
 * @param input Data tree specifying the input of the RPC/action.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
 * Points to the operation root node.
 *
 * @return Error code.
 *
 */
sr::ErrorCode SetCurrentDatetimeRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
    sr::Event event, uint32_t requestId, ly::DataNode output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    auto current_datetime = input.findPath("current-datetime");

    if (current_datetime) {
        auto datetime = std::get<std::string>(current_datetime->asTerm().value());

        try {
            ietf::sys::state::setCurrentDatetime(datetime);
        } catch (const std::runtime_error& err) {
            error = sr::ErrorCode::OperationFailed;
        }
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin RPC context.
 *
 */
SystemRestartRpcCb::SystemRestartRpcCb(std::shared_ptr<SystemRpcContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-restart.
 *
 * A callback for RPC/action subscriptions.
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param path Path identifying the RPC/action.
 * @param input Data tree specifying the input of the RPC/action.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
 * Points to the operation root node.
 *
 * @return Error code.
 *
 */
sr::ErrorCode SystemRestartRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
    sr::Event event, uint32_t requestId, ly::DataNode output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    try {
        ietf::sys::state::restartSystem();
    } catch (const std::runtime_error& err) {
        error = sr::ErrorCode::OperationFailed;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin RPC context.
 *
 */
SystemShutdownRpcCb::SystemShutdownRpcCb(std::shared_ptr<SystemRpcContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-shutdown.
 *
 * A callback for RPC/action subscriptions.
 * @param session An implicit session for the callback.
 * @param subscriptionId ID the subscription associated with the callback.
 * @param path Path identifying the RPC/action.
 * @param input Data tree specifying the input of the RPC/action.
 * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
 * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
 * Points to the operation root node.
 *
 * @return Error code.
 *
 */
sr::ErrorCode SystemShutdownRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
    sr::Event event, uint32_t requestId, ly::DataNode output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    try {
        ietf::sys::state::shutdownSystem();
    } catch (const std::runtime_error& err) {
        error = sr::ErrorCode::OperationFailed;
    }

    return error;
}

}

/**
 * System module constructor. Allocates each context.
 */
SystemModule::SystemModule(ietf::sys::PluginContext& plugin_ctx)
    : srpc::IModule<ietf::sys::PluginContext>(plugin_ctx)
{
    m_operContext = std::make_shared<SystemOperationalContext>();
    m_changeContext = std::make_shared<SystemModuleChangesContext>();
    m_rpcContext = std::make_shared<SystemRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> SystemModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> SystemModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> SystemModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> SystemModule::getOperationalCallbacks()
{
    return {
        srpc::OperationalCallback { "/ietf-system:system-state/platform", ietf::sys::sub::oper::StatePlatformOperGetCb(m_operContext) },
        srpc::OperationalCallback { "/ietf-system:system-state/clock", ietf::sys::sub::oper::StateClockOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> SystemModule::getModuleChangeCallbacks() { return {}; }

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> SystemModule::getRpcCallbacks()
{
    return {
        srpc::RpcCallback { "/ietf-system:system-restart", ietf::sys::sub::rpc::SystemRestartRpcCb(m_rpcContext) },
        srpc::RpcCallback { "/ietf-system:system-shutdown", ietf::sys::sub::rpc::SystemShutdownRpcCb(m_rpcContext) },
        srpc::RpcCallback { "/ietf-system:set-current-datetime", ietf::sys::sub::rpc::SetCurrentDatetimeRpcCb(m_rpcContext) },
    };
}

/**
 * Get module name.
 */
constexpr const char* SystemModule::getName() { return "System"; }
