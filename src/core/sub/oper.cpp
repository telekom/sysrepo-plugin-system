#include "oper.hpp"

// Platform information
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys {
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

        struct utsname uname_data = { 0 };

        if (uname(&uname_data) < 0) {
            return sr::ErrorCode::Internal;
        }

        auto platform = output->newPath("platform");

        if (platform) {
            platform->newPath("os-name", uname_data.sysname);
            platform->newPath("os-release", uname_data.release);
            platform->newPath("os-version", uname_data.version);
            platform->newPath("machine", uname_data.machine);
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