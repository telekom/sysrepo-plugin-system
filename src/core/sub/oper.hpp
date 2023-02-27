#pragma once

#include "core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>
#include <libyang-cpp/Context.hpp>

namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::sys {
namespace sub::oper {
    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform/os-name.
     */
    class PlatformOsNameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        PlatformOsNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform/os-name.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform/os-release.
     */
    class PlatformOsReleaseOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        PlatformOsReleaseOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform/os-release.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform/os-version.
     */
    class PlatformOsVersionOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        PlatformOsVersionOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform/os-version.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform/machine.
     */
    class PlatformMachineOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        PlatformMachineOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform/machine.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/clock/current-datetime.
     */
    class ClockCurrentDatetimeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ClockCurrentDatetimeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/clock/current-datetime.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/clock/boot-datetime.
     */
    class ClockBootDatetimeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ClockBootDatetimeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/clock/boot-datetime.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

    private:
        std::shared_ptr<ietf::sys::OperContext> m_ctx;
    };

}
}