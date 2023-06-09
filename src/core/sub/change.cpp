#include "change.hpp"
// dns features
#include "core/system/dns.hpp"

#include "core/common.hpp"
#include "core/api.hpp"

// system API
#include <core/system/hostname.hpp>
#include <core/system/timezone-name.hpp>
#include <core/system/auth.hpp>

// logging
#include <sysrepo.h>

// path handling
#include <filesystem>

namespace ietf::sys {
namespace sub::change {
    // use system API
    namespace sys = ietf::sys;

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    ContactModuleChangeCb::ContactModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode ContactModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    HostnameModuleChangeCb::HostnameModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
                case sysrepo::ChangeOperation::Modified: {
                    auto value = change.node.asTerm().value();
                    auto hostname = std::get<std::string>(value);

                    try {
                        Hostname host;
                        host.setHostname(hostname);
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
    LocationModuleChangeCb::LocationModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode LocationModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    ClockTimezoneNameModuleChangeCb::ClockTimezoneNameModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
                    auto value = change.node.asTerm().value();
                    auto timezone = std::get<std::string>(value);

                    try {
                        Timezone t;
                        t.setTimezone(timezone);
                    } catch (std::runtime_error& e) {
                        SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "%s", e.what());
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
    ClockTimezoneUtcOffsetModuleChangeCb::ClockTimezoneUtcOffsetModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpEnabledModuleChangeCb::NtpEnabledModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode NtpEnabledModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerModuleChangeCb::NtpServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode NtpServerModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    DnsSearchModuleChangeCb::DnsSearchModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode DnsSearchModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        dns::DnsSearchServerList dnsSearchServers;
        // get all from bus

        switch (event) {
        case sysrepo::Event::Change:

            if (dnsSearchServers.importListFromSdBus()) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "%s", "sd bus import failed!");
                return sr::ErrorCode::OperationFailed;
            }

            // if the call succseeded - continue
            for (auto& change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {

                    // get the changed values in node
                    auto value = change.node.asTerm().value();
                    auto domain = std::get<std::string>(value);

                    // no value provided for search param - default true? or maybe another param?
                    sys::dns::DnsSearchServer server(domain, true);
                    dnsSearchServers.addDnsSearchServer(server);

                    break;
                }
                case sysrepo::ChangeOperation::Deleted: {
                    // modification:
                    // first it goes to delete event, then create
                    // take deleted from here

                    auto deletedValue = change.node.asTerm().value();
                    auto deletedDomain = std::get<std::string>(deletedValue);

                    sys::dns::DnsSearchServer deleted(deletedDomain, true);

                    dnsSearchServers.removeDnsSearchServer(deleted);

                    break;
                }

                case sysrepo::ChangeOperation::Moved:
                    break;
                }
            }

            // process finished -> export
            try {
                dnsSearchServers.exportListToSdBus();
            } catch (sdbus::Error& e) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "%s", e.getMessage().c_str());
                return sr::ErrorCode::OperationFailed;
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
    DnsServerModuleChangeCb::DnsServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode DnsServerModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        dns::DnsServerList dnsList;
        switch (event) {
        case sysrepo::Event::Change:

            // first take all from sdbus
            if (dnsList.importListFromSdBus()) {
                SRPLG_LOG_ERR(PLUGIN_NAME, "%s", "sd bus import failed!");
                return sr::ErrorCode::OperationFailed;
            }

            for (sysrepo::Change change : session.getChanges(subXPath->data())) {
                switch (change.operation) {
                case sysrepo::ChangeOperation::Created: {

                    std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

                    if (server == std::nullopt) {
                        error = sr::ErrorCode::OperationFailed;

                    } else {
                        dnsList.addDnsServer(server.value());
                        error = sr::ErrorCode::Ok;
                    }
                    break;
                }

                case sysrepo::ChangeOperation::Modified: {

                    std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

                    if (server == std::nullopt) {
                        error = sr::ErrorCode::OperationFailed;

                    } else {
                        // modify here
                        try {
                            dnsList.modifyDnsServer(server.value());
                        }catch(std::exception(&e)){
                            SRPLG_LOG_ERR(PLUGIN_NAME, "%s", e.what());
                            return sr::ErrorCode::OperationFailed;
                        };

                            error = sr::ErrorCode::Ok;
                        }
                        break;
                    }

                case sysrepo::ChangeOperation::Deleted: {

                    std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

                    if (server == std::nullopt) {
                        error = sr::ErrorCode::OperationFailed;

                    } else {
                        dnsList.removeDnsServer(server.value());
                        error = sr::ErrorCode::Ok;
                    }

                    // // deleted code here
                    break;
                }

                default:
                    break;
                }
                }
                if (dnsList.exportListToSdBus() == true) {
                    return sr::ErrorCode::OperationFailed;
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
        DnsTimeoutModuleChangeCb::DnsTimeoutModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
        sr::ErrorCode DnsTimeoutModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        DnsAttemptsModuleChangeCb::DnsAttemptsModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
        sr::ErrorCode DnsAttemptsModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusServerModuleChangeCb::RadiusServerModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
        sr::ErrorCode RadiusServerModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusTimeoutModuleChangeCb::RadiusTimeoutModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
        sr::ErrorCode RadiusTimeoutModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        RadiusAttemptsModuleChangeCb::RadiusAttemptsModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

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
        sr::ErrorCode RadiusAttemptsModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        AuthUserAuthenticationOrderModuleChangeCb::AuthUserAuthenticationOrderModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx)
        {
            m_ctx = ctx;
        }

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user-authentication-order.
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
        sr::ErrorCode AuthUserAuthenticationOrderModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }

        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin module change context.
         *
         */
        AuthUserModuleChangeCb::AuthUserModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

        /**
         * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user[name='%s'].
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
        sr::ErrorCode AuthUserModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
            std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
        {
            sr::ErrorCode error = sr::ErrorCode::Ok;
            return error;
        }
    }
}