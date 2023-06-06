#include "change.hpp"

#include "core/common.hpp"
#include "core/api.hpp"
#include "libyang-cpp/Enum.hpp"

// system API
#include <core/system/hostname.hpp>
#include <core/system/timezone-name.hpp>
#include <core/system/auth.hpp>

// sethostname() and gethostname()
#include <unistd.h>

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
    AuthUserNameModuleChangeCb::AuthUserNameModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user[name='%s']/name.
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
    sr::ErrorCode AuthUserNameModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.path().c_str());
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

                SRPLG_LOG_DBG(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                    // create user
                    break;
                case sysrepo::ChangeOperation::Modified:
                    // should not be possible to edit name
                    break;
                case sysrepo::ChangeOperation::Deleted:
                    // delete user
                    break;
                case sysrepo::ChangeOperation::Moved:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return sr::ErrorCode::CallbackFailed;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    AuthUserPasswordModuleChangeCb::AuthUserPasswordModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user[name='%s']/password.
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
    sr::ErrorCode AuthUserPasswordModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.path().c_str());
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

                SRPLG_LOG_DBG(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                    // create user password
                    break;
                case sysrepo::ChangeOperation::Modified:
                    // modify user password
                    break;
                case sysrepo::ChangeOperation::Deleted:
                    // delete user or delete user password
                    break;
                case sysrepo::ChangeOperation::Moved:
                    break;
                }
            }
            break;
        default:
            break;
        }

        return sr::ErrorCode::CallbackFailed;
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

        switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.path().c_str());
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

                SRPLG_LOG_DBG(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                // const auto& name_node = change.node.findPath("name");
                // const auto& password_node = change.node.findPath("password");

                // const auto& user_meta = ietf::sys::getMetaValuesHash(change.node.meta());
                // const auto& name_meta = ietf::sys::getMetaValuesHash(name_node->meta());
                // auto password_meta = std::map<std::string, std::string>();

                // const auto& name_value = name_node->asTerm().value();
                // const auto& name = std::get<std::string>(name_value);
                // auto password = std::optional<std::string>(std::nullopt);

                // if (password_node) {
                //     const auto& password_value = password_node->asTerm().value();
                //     password = std::get<std::string>(password_value);
                //     password_meta = ietf::sys::getMetaValuesHash(password_node->meta());
                // }

                // switch (change.operation) {
                // case sysrepo::ChangeOperation::Created: {
                //     // create new user or create new password
                //     const auto& name_operation = name_meta.find("operation");
                //     if (name_operation != name_meta.end()) {
                //         // name has operation
                //     } else {
                //         // no name operation - only password change
                //     }
                //     break;
                // }
                // case sysrepo::ChangeOperation::Modified:
                //     // modify user password
                //     break;
                // case sysrepo::ChangeOperation::Deleted:
                //     // delete user or delete user password
                //     break;
                // case sysrepo::ChangeOperation::Moved:
                //     break;
                // }
            }
            break;
        default:
            break;
        }

        return sr::ErrorCode::CallbackFailed;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    AuthUserAuthorizedKeyModuleChangeCb::AuthUserAuthorizedKeyModuleChangeCb(std::shared_ptr<ietf::sys::ModuleChangeContext> ctx) { m_ctx = ctx; }

    /**
     * Functor for path /ietf-system:system/authentication/user/authorized-key.
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
    sr::ErrorCode AuthUserAuthorizedKeyModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

                SRPLG_LOG_DBG(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                SRPLG_LOG_DBG(PLUGIN_NAME, "Node path: %s", change.node.path().data());

                const auto& user_name = ietf::sys::extractListKeyFromXPath("user", "name", change.node.path());

                SRPLG_LOG_DBG(PLUGIN_NAME, "Username for authorized key: %s", user_name.data());

                switch (change.operation) {
                case sysrepo::ChangeOperation::Created:
                case sysrepo::ChangeOperation::Modified: {
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

        return sr::ErrorCode::CallbackFailed;
    }
}
}
