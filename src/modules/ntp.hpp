#include "core/context.hpp"
#include "srpcpp/datastore.hpp"
#include <srpcpp/module.hpp>
#include <srpcpp/module-registry.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include <string>
#include <list>
#include <optional>

namespace sr = sysrepo;
namespace ly = libyang;

/**
 * Operational context for the NTP module.
 */
class NtpOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the NTP module.
 */
class NtpModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the NTP module.
 */
class NtpRpcContext : public srpc::IModuleContext { };

namespace ietf::sys::ntp {
struct NtpServer {
    std::string Name;
    std::string Address;
    std::string Port;
    std::string AssociationType;
    std::string IBurst;
    std::string Prefer;
};

class NtpServerList {
public:
    /**
     * @brief Constructor for the list of ntp servers. Uses the session and augyang ability to retrieve NTP servers on the system.
     */
    NtpServerList(sr::Session& sess);

    /**
     * @brief Load the list from the system.
     */
    void loadFromSystem();

    /**
     * @brief Store the list to the system.
     */
    void storeToSystem();

private:
    std::list<NtpServer> m_servers; ///< List of NTP servers to manipulate.
    sr::Session& m_session;
};
}

namespace ietf::sys::ntp::change {

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
    NtpEnabledModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/name.
 */
class NtpServerNameModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerNameModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/name.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/udp/address.
 */
class NtpServerUdpAddressModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerUdpAddressModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/udp/address.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/udp/port.
 */
class NtpServerUdpPortModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerUdpPortModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/udp/port.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/udp.
 */
class NtpServerUdpModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerUdpModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/udp.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/association-type.
 */
class NtpServerAssociationTypeModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerAssociationTypeModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/association-type.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/iburst.
 */
class NtpServerIburstModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerIburstModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/iburst.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp/server[name='%s']/prefer.
 */
class NtpServerPreferModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpServerPreferModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp/server[name='%s']/prefer.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
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
    NtpServerModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/ntp.
 */
class NtpModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    NtpModuleChangeCb(std::shared_ptr<NtpModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/ntp.
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
    std::shared_ptr<NtpModuleChangesContext> m_ctx;
};
}

namespace ietf::sys::ntp::oper {
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
    NtpEnabledOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerNameOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerUdpAddressOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerUdpPortOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerUdpOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerAssociationTypeOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerIburstOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerPreferOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpServerOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
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
    NtpOperGetCb(std::shared_ptr<NtpOperationalContext> ctx);

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
    std::shared_ptr<NtpOperationalContext> m_ctx;
};
}

/**
 * @brief Applier used to apply /ietf-system:system/ntp values from the datastore to the system.
 */
class NtpValuesApplier : public srpc::IDatastoreApplier {
    /**
     * @brief Apply datastore content from the provided session to the system.
     *
     * @param session Session to use for retreiving datastore data.
     */
    virtual void applyDatastoreValues(sysrepo::Session& session) override;

    /**
     * @brief Get the paths which the checker/applier is assigned for.
     *
     * @return Assigned paths.
     */
    virtual std::list<std::string> getPaths() override
    {
        return {
            "/ietf-system:system/ntp",
        };
    }
};

/**
 * @brief NTP container module.
 */
class NtpModule : public srpc::IModule<ietf::sys::PluginContext> {
public:
    /**
     * NTP module constructor. Allocates each context.
     */
    NtpModule(ietf::sys::PluginContext& plugin_ctx);

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<srpc::OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<srpc::ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<srpc::RpcCallback> getRpcCallbacks() override;

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * Auth module destructor.
     */
    ~NtpModule() { }

private:
    std::shared_ptr<NtpOperationalContext> m_operContext;
    std::shared_ptr<NtpModuleChangesContext> m_changeContext;
    std::shared_ptr<NtpRpcContext> m_rpcContext;
};
