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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/contact.
     */
    class ContactOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ContactOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/contact.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/location.
     */
    class LocationOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        LocationOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/location.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/clock.
     */
    class ClockOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        ClockOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/clock.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/enabled.
     */
    class NtpEnabledOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpEnabledOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/enabled.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/name.
     */
    class NtpServerNameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/name.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/udp/address.
     */
    class NtpServerUdpAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerUdpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/udp/address.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/udp/port.
     */
    class NtpServerUdpPortOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerUdpPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/udp/port.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/udp.
     */
    class NtpServerUdpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerUdpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/udp.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/association-type.
     */
    class NtpServerAssociationTypeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerAssociationTypeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/association-type.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/iburst.
     */
    class NtpServerIburstOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerIburstOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/iburst.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s']/prefer.
     */
    class NtpServerPreferOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerPreferOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s']/prefer.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp/server[name='%s'].
     */
    class NtpServerOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp/server[name='%s'].
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/ntp.
     */
    class NtpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        NtpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/ntp.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/search.
     */
    class DnsSearchOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsSearchOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/search.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/server[name='%s']/name.
     */
    class DnsServerNameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/server[name='%s']/name.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/address.
     */
    class DnsServerUdpAndTcpAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsServerUdpAndTcpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/address.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/port.
     */
    class DnsServerUdpAndTcpPortOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsServerUdpAndTcpPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp/port.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp.
     */
    class DnsServerUdpAndTcpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsServerUdpAndTcpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/server[name='%s']/udp-and-tcp.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/server[name='%s'].
     */
    class DnsServerOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/server[name='%s'].
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/options/timeout.
     */
    class DnsOptionsTimeoutOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsOptionsTimeoutOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/options/timeout.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/options/attempts.
     */
    class DnsOptionsAttemptsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsOptionsAttemptsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/options/attempts.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver/options.
     */
    class DnsOptionsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsOptionsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver/options.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/dns-resolver.
     */
    class DnsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        DnsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/dns-resolver.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s']/name.
     */
    class RadiusServerNameOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerNameOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s']/name.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s']/udp/address.
     */
    class RadiusServerUdpAddressOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerUdpAddressOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s']/udp/address.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path
     * /ietf-system:system/radius/server[name='%s']/udp/authentication-port.
     */
    class RadiusServerUdpAuthenticationPortOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerUdpAuthenticationPortOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path
         * /ietf-system:system/radius/server[name='%s']/udp/authentication-port.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s']/udp/shared-secret.
     */
    class RadiusServerUdpSharedSecretOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerUdpSharedSecretOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s']/udp/shared-secret.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s']/udp.
     */
    class RadiusServerUdpOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerUdpOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s']/udp.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s']/authentication-type.
     */
    class RadiusServerAuthenticationTypeOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerAuthenticationTypeOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s']/authentication-type.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/server[name='%s'].
     */
    class RadiusServerOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusServerOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/server[name='%s'].
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/options/timeout.
     */
    class RadiusOptionsTimeoutOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusOptionsTimeoutOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/options/timeout.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/options/attempts.
     */
    class RadiusOptionsAttemptsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusOptionsAttemptsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/options/attempts.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius/options.
     */
    class RadiusOptionsOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusOptionsOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius/options.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/radius.
     */
    class RadiusOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        RadiusOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/radius.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system.
     */
    class SystemOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        SystemOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform.
     */
    class StatePlatformOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StatePlatformOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform.
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

    /**
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/clock.
     */
    class StateClockOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StateClockOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/clock.
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
     * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state.
     */
    class StateOperGetCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin operational context.
         *
         */
        StateOperGetCb(std::shared_ptr<ietf::sys::OperContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state.
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