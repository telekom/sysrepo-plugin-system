#include "oper.hpp"

// Platform information
#include <stdexcept>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys {

/**
 * @brief Platform information helper struct.
 */
struct PlatformInfo {
    std::string OsName; ///< OS name.
    std::string OsRelease; ///< OS release.
    std::string OsVersion; ///< OS version.
    std::string Machine; ///< Machine.
};

/**
 * @brief Clock information helper struct.
 */
struct ClockInfo {
    std::string BootDatetime; ///< Boot datetime.
    std::string CurrentDatetime; ///< Current datetime.
};

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

namespace sub::oper {
    /**
     * Default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    PlatformOperGetCb::PlatformOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * operator() for path /ietf-system:system-state/platform.
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
    sr::ErrorCode PlatformOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        auto platform_info = ietf::sys::getPlatformInfo();

        auto platform = output->newPath("platform");

        if (platform) {
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
    PlatformOsNameOperGetCb::PlatformOsNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/platform/os-name.
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
    sr::ErrorCode PlatformOsNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        struct utsname uname_data = { 0 };

        if (uname(&uname_data) < 0) {
            return sr::ErrorCode::Internal;
        }

        output->newPath("os-name", uname_data.sysname);

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    PlatformOsReleaseOperGetCb::PlatformOsReleaseOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/platform/os-release.
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
    sr::ErrorCode PlatformOsReleaseOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        struct utsname uname_data = { 0 };

        if (uname(&uname_data) < 0) {
            return sr::ErrorCode::Internal;
        }

        output->newPath("os-release", uname_data.release);

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    PlatformOsVersionOperGetCb::PlatformOsVersionOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/platform/os-version.
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
    sr::ErrorCode PlatformOsVersionOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        struct utsname uname_data = { 0 };

        if (uname(&uname_data) < 0) {
            return sr::ErrorCode::Internal;
        }

        output->newPath("os-version", uname_data.version);

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    PlatformMachineOperGetCb::PlatformMachineOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/platform/machine.
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
    sr::ErrorCode PlatformMachineOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        struct utsname uname_data = { 0 };

        if (uname(&uname_data) < 0) {
            return sr::ErrorCode::Internal;
        }

        output->newPath("machine", uname_data.machine);

        return error;
    }

    /**
     * Default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ClockOperGetCb::ClockOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * Operational get operator() for path /ietf-system:system-state/clock.
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
    sr::ErrorCode ClockOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ClockCurrentDatetimeOperGetCb::ClockCurrentDatetimeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/clock/current-datetime.
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
    sr::ErrorCode ClockCurrentDatetimeOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ClockBootDatetimeOperGetCb::ClockBootDatetimeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state/clock/boot-datetime.
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
    sr::ErrorCode ClockBootDatetimeOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

}
}