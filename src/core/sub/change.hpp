#pragma once

#include "core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>

namespace sr = sysrepo;

namespace ietf::sys {
namespace sub::change {
    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/contact.
     */
    class ContactModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ContactModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/contact.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/location.
     */
    class LocationModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        LocationModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/location.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/clock/timezone-name.
     */
    class ClockTimezoneNameModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ClockTimezoneNameModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/clock/timezone-utc-offset.
     */
    class ClockTimezoneUtcOffsetModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        ClockTimezoneUtcOffsetModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/enabled.
     */
    class NtpEnabledModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        NtpEnabledModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/enabled.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s'].
     */
    class NtpServerModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        NtpServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/dns-resolver/search.
     */
    class DnsSearchModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        DnsSearchModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/dns-resolver/search.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/dns-resolver/server[name='%s'].
     */
    class DnsServerModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        DnsServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/dns-resolver/server[name='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/dns-resolver/options/timeout.
     */
    class DnsTimeoutModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        DnsTimeoutModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/dns-resolver/options/timeout.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/dns-resolver/options/attempts.
     */
    class DnsAttemptsModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        DnsAttemptsModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/dns-resolver/options/attempts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/radius/server[name='%s'].
     */
    class RadiusServerModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/radius/server[name='%s'].
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/radius/options/timeout.
     */
    class RadiusTimeoutModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusTimeoutModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/radius/options/timeout.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/radius/options/attempts.
     */
    class RadiusAttemptsModuleChangeCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusAttemptsModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/radius/options/attempts.
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
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
            sr::Event event, uint32_t requestId);

    private:
        std::shared_ptr<ietf::sys::ModuleChangeContext> m_ctx;
    };

}
}