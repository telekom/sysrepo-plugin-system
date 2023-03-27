#include "oper.hpp"

#include "core/common.hpp"
#include "core/types.hpp"
#include "core/api.hpp"

// system-state
#include <core/system-state/platform.hpp>
#include <core/system-state/clock.hpp>

// system
#include <core/system/hostname.hpp>
#include <core/system/timezone-name.hpp>
#include <core/system/auth.hpp>

// Platform information
#include <sstream>
#include <stdexcept>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys {
namespace sub::oper {
    // use API namespace in operational callbacks
    namespace sys_state = ietf::sys::state;
    namespace sys = ietf::sys;

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ContactOperGetCb::ContactOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/contact.
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
    sr::ErrorCode ContactOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    HostnameOperGetCb::HostnameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

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
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        auto hostname = sys::getHostname();

        output->newPath("hostname", hostname);

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    LocationOperGetCb::LocationOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/location.
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
    sr::ErrorCode LocationOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    ClockTimezoneNameOperGetCb::ClockTimezoneNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

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
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
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
    ClockTimezoneUtcOffsetOperGetCb::ClockTimezoneUtcOffsetOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

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
    ClockOperGetCb::ClockOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/clock.
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
    NtpEnabledOperGetCb::NtpEnabledOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/enabled.
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
    sr::ErrorCode NtpEnabledOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerNameOperGetCb::NtpServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/name.
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
    sr::ErrorCode NtpServerNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerUdpAddressOperGetCb::NtpServerUdpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/udp/address.
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
    sr::ErrorCode NtpServerUdpAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerUdpPortOperGetCb::NtpServerUdpPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/udp/port.
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
    sr::ErrorCode NtpServerUdpPortOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerUdpOperGetCb::NtpServerUdpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/udp.
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
    sr::ErrorCode NtpServerUdpOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerAssociationTypeOperGetCb::NtpServerAssociationTypeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/association-type.
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
    sr::ErrorCode NtpServerAssociationTypeOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerIburstOperGetCb::NtpServerIburstOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/iburst.
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
    sr::ErrorCode NtpServerIburstOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerPreferOperGetCb::NtpServerPreferOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s']/prefer.
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
    sr::ErrorCode NtpServerPreferOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpServerOperGetCb::NtpServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp/server[name='%s'].
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
    sr::ErrorCode NtpServerOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    NtpOperGetCb::NtpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/ntp.
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
    sr::ErrorCode NtpOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsSearchOperGetCb::DnsSearchOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/search.
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
    sr::ErrorCode DnsSearchOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsServerNameOperGetCb::DnsServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/server[name='%s']/name.
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
    sr::ErrorCode DnsServerNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsServerUdpAndTcpAddressOperGetCb::DnsServerUdpAndTcpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/address.
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
    sr::ErrorCode DnsServerUdpAndTcpAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsServerUdpAndTcpPortOperGetCb::DnsServerUdpAndTcpPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/port.
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
    sr::ErrorCode DnsServerUdpAndTcpPortOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsServerUdpAndTcpOperGetCb::DnsServerUdpAndTcpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp.
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
    sr::ErrorCode DnsServerUdpAndTcpOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsServerOperGetCb::DnsServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/server[name='%s'].
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
    sr::ErrorCode DnsServerOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsOptionsTimeoutOperGetCb::DnsOptionsTimeoutOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/options/timeout.
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
    sr::ErrorCode DnsOptionsTimeoutOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsOptionsAttemptsOperGetCb::DnsOptionsAttemptsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/options/attempts.
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
    sr::ErrorCode DnsOptionsAttemptsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsOptionsOperGetCb::DnsOptionsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver/options.
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
    sr::ErrorCode DnsOptionsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    DnsOperGetCb::DnsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/dns-resolver.
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
    sr::ErrorCode DnsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerNameOperGetCb::RadiusServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/name.
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
    sr::ErrorCode RadiusServerNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerUdpAddressOperGetCb::RadiusServerUdpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/udp/address.
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
    sr::ErrorCode RadiusServerUdpAddressOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerUdpAuthenticationPortOperGetCb::RadiusServerUdpAuthenticationPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx)
    {
        m_ctx = ctx;
    }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/udp/authentication-port.
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
    sr::ErrorCode RadiusServerUdpAuthenticationPortOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerUdpSharedSecretOperGetCb::RadiusServerUdpSharedSecretOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/udp/shared-secret.
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
    sr::ErrorCode RadiusServerUdpSharedSecretOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerUdpOperGetCb::RadiusServerUdpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/udp.
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
    sr::ErrorCode RadiusServerUdpOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerAuthenticationTypeOperGetCb::RadiusServerAuthenticationTypeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s']/authentication-type.
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
    sr::ErrorCode RadiusServerAuthenticationTypeOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusServerOperGetCb::RadiusServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/server[name='%s'].
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
    sr::ErrorCode RadiusServerOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusOptionsTimeoutOperGetCb::RadiusOptionsTimeoutOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/options/timeout.
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
    sr::ErrorCode RadiusOptionsTimeoutOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusOptionsAttemptsOperGetCb::RadiusOptionsAttemptsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/options/attempts.
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
    sr::ErrorCode RadiusOptionsAttemptsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusOptionsOperGetCb::RadiusOptionsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius/options.
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
    sr::ErrorCode RadiusOptionsOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    RadiusOperGetCb::RadiusOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/radius.
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
    sr::ErrorCode RadiusOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserAuthenticationOrderOperGetCb::AuthUserAuthenticationOrderOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user-authentication-order.
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
    sr::ErrorCode AuthUserAuthenticationOrderOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserNameOperGetCb::AuthUserNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/name.
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
    sr::ErrorCode AuthUserNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserPasswordOperGetCb::AuthUserPasswordOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/password.
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
    sr::ErrorCode AuthUserPasswordOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserAuthorizedKeyNameOperGetCb::AuthUserAuthorizedKeyNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/name.
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
    sr::ErrorCode AuthUserAuthorizedKeyNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserAuthorizedKeyAlgorithmOperGetCb::AuthUserAuthorizedKeyAlgorithmOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm.
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
    sr::ErrorCode AuthUserAuthorizedKeyAlgorithmOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserAuthorizedKeyKeyDataOperGetCb::AuthUserAuthorizedKeyKeyDataOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data.
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
    sr::ErrorCode AuthUserAuthorizedKeyKeyDataOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserAuthorizedKeyOperGetCb::AuthUserAuthorizedKeyOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s'].
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
    sr::ErrorCode AuthUserAuthorizedKeyOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    AuthUserOperGetCb::AuthUserOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s'].
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
    sr::ErrorCode AuthUserOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        auto users = sys::auth::getLocalUserList();

        // traverse users and collect authorized-key lists
        for (auto& user : users) {
            try {
                user.AuthorizedKeys = sys::auth::getAuthorizedKeyList(user.Name);
            } catch (...) {
            }
        }

        // create YANG subtree
        for (const auto& user : users) {
            std::stringstream path_buffer;

            path_buffer << "user[name='" << user.Name << "']";

            auto user_node = output->newPath(path_buffer.str());
            if (user_node) {
                if (user.Password) {
                    user_node->newPath("password", user.Password);
                }

                // create authorized-key subtree
                if (user.AuthorizedKeys) {
                    for (const auto& key : user.AuthorizedKeys.value()) {
                        std::stringstream key_path_buffer;

                        key_path_buffer << "authorized-key[name='" << key.Name << "']";

                        auto key_node = user_node->newPath(key_path_buffer.str());
                        if (key_node) {
                            key_node->newPath("algorithm", key.Algorithm);
                            key_node->newPath("key-data", key.Data);
                        } else {
                            error = sr::ErrorCode::Internal;
                            break;
                        }
                    }
                }
            } else {
                error = sr::ErrorCode::Internal;
                break;
            }
        }

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthOperGetCb::AuthOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication.
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
    sr::ErrorCode AuthOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
    SystemOperGetCb::SystemOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system.
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
    sr::ErrorCode SystemOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
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
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    StatePlatformOperGetCb::StatePlatformOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

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
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

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

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    StateClockOperGetCb::StateClockOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

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
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;

        auto clock = output->newPath("clock");

        if (clock) {
            auto clock_info = sys_state::getClockInfo();

            clock->newPath("current-datetime", clock_info.CurrentDatetime);
            clock->newPath("boot-datetime", clock_info.BootDatetime);
        }

        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    StateOperGetCb::StateOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx) { m_ctx = ctx; }

    /**
     * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system-state.
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
    sr::ErrorCode StateOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
        std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId,
        std::optional<ly::DataNode>& output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

}
}