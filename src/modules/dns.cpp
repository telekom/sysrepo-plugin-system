#include "dns.hpp"
#include "core/common.hpp"
#include "core/ip.hpp"
#include "srpcpp/common.hpp"
#include "sysrepo-cpp/Enum.hpp"

#include <core/sdbus.hpp>

// logging
#include <stdexcept>
#include <sysrepo.h>

namespace ietf::sys::dns {

/**
 * @breif Default constructor.
 */
DnsServerList::DnsServerList()
    : SdBus("org.freedesktop.resolve1", "/org/freedesktop/resolve1", "org.freedesktop.resolve1.Manager", "SetLinkDNSEx", "DNSEx")
    , m_ifindex(SYSTEMD_IFINDEX)
{
}

/**
 * @brief Loads the list of DNS servers found currently on the system.
 */
void DnsServerList::loadFromSystem()
{
    // convert from SDBus list to our list
    auto servers = this->importFromSdBus();
    for (auto& s : servers) {
        const auto addr_type = s.get<1>();

        switch (addr_type) {
            case AF_INET:
                m_servers.push_back(DnsServer {
                    .InterfaceIndex = s.get<0>(),
                    .Address = std::make_unique<ip::Ipv4Address>(s.get<2>()),
                    .Port = s.get<3>(),
                    .Name = s.get<4>(),
                });
                break;
            case AF_INET6:
                m_servers.push_back(DnsServer {
                    .InterfaceIndex = s.get<0>(),
                    .Address = std::make_unique<ip::Ipv6Address>(s.get<2>()),
                    .Port = s.get<3>(),
                    .Name = s.get<4>(),
                });
                break;
            default:
                break;
        }
    }
}

/**
 * @brief Stores the list of DNS servers in the class to the system.
 */
void DnsServerList::storeToSystem()
{
    // convert to SDBus list and store it
    std::vector<sdbus::Struct<int32_t, std::vector<uint8_t>, uint16_t, std::string>> sdbus_data;

    for (auto& server : m_servers) {
        sdbus_data.push_back(sdbus::Struct<int32_t, std::vector<uint8_t>, uint16_t, std::string>(
            server.Address->getVersion(), server.Address->asBytes(), server.Port, server.Name));
    }

    this->exportToSdBus(m_ifindex, sdbus_data);
}

/**
 * @brief Default constructor.
 */
DnsSearchList::DnsSearchList()
    : SdBus("org.freedesktop.resolve1", "/org/freedesktop/resolve1", "org.freedesktop.resolve1.Manager", "SetLinkDomains", "Domains")
    , m_ifindex(SYSTEMD_IFINDEX)
{
}

/**
 * @brief Loads the list of DNS servers found currently on the system.
 */
void DnsSearchList::loadFromSystem()
{
    // convert from SDBus list to our list
    auto domains = this->importFromSdBus();
    for (auto& d : domains) {
        m_search.push_back(DnsSearch {
            .InterfaceIndex = d.get<0>(),
            .Domain = d.get<1>(),
            .Search = d.get<2>(),
        });
    }
}

/**
 * @brief Add new search domain to the list.
 *
 * @param domain Search domain to create.
 */
void DnsSearchList::createSearchDomain(const std::string& domain)
{
    m_search.push_back(DnsSearch { .InterfaceIndex = SYSTEMD_IFINDEX, .Domain = domain, .Search = false });
}

/**
 * @brief Delete search domain from the list.
 *
 * @param domain Search domain to remove.
 */
void DnsSearchList::deleteSearchDomain(const std::string& domain)
{
    m_search.remove_if([&domain](const auto& search) { return search.Domain == domain; });
}

/**
 * @brief Stores the list of DNS servers in the class to the system.
 */
void DnsSearchList::storeToSystem()
{
    // convert to SDBus list and store it
    std::vector<sdbus::Struct<std::string, bool>> sdbus_data;

    for (auto& search : m_search) {
        sdbus_data.push_back(sdbus::Struct<std::string, bool>(sdbus::make_struct(search.Domain, search.Search)));
    }

    this->exportToSdBus(m_ifindex, sdbus_data);
}
} // end of namespace

namespace ietf::sys::sub::oper {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DnsSearchOperGetCb::DnsSearchOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    dns::DnsSearchList search_list;

    // load search list and add it to the output yang
    try {
        search_list.loadFromSystem();

        // iterate list and add elements to the output tree
        for (auto& iter : search_list) {
            output->newPath("search", iter.Domain);
        }
    } catch (const std::runtime_error& err) {
        SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "Unable to load DNS search list from the system: %s", err.what());
        error = sr::ErrorCode::OperationFailed;
    }

    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DnsServerNameOperGetCb::DnsServerNameOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsServerUdpAndTcpAddressOperGetCb::DnsServerUdpAndTcpAddressOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsServerUdpAndTcpPortOperGetCb::DnsServerUdpAndTcpPortOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsServerUdpAndTcpOperGetCb::DnsServerUdpAndTcpOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsServerOperGetCb::DnsServerOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    dns::DnsServerList servers;

    try {
        servers.loadFromSystem();

        for (auto& server : servers) {
            std::stringstream path_buffer;

            path_buffer << "server[name='" << server.Name << "']";

            auto server_node = output->newPath(path_buffer.str());
            if (server_node) {
                if (server.Port != 0) {
                    server_node->newPath("port", std::to_string(server.Port));
                }
                server_node->newPath("udp-and-tcp/address", server.Address->asString());
            } else {
                SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "Unable to create a new server node");
                error = sr::ErrorCode::Internal;
                break;
            }
        }
    } catch (const std::exception& e) {
        SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "Error loading DNS server list from the system");
        SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "%s", e.what());
        error = sr::ErrorCode::Internal;
    }
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
DnsOptionsTimeoutOperGetCb::DnsOptionsTimeoutOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsOptionsAttemptsOperGetCb::DnsOptionsAttemptsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsOptionsOperGetCb::DnsOptionsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
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
DnsOperGetCb::DnsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx) { m_ctx = ctx; }

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
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}
}

namespace ietf::sys::sub::change {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
DnsSearchModuleChangeCb::DnsSearchModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx) { m_ctx = ctx; }

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
    dns::DnsSearchList search_list;

    try {
        search_list.loadFromSystem();
    } catch (const std::runtime_error& err) {
        SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "Unable to load DNS search domains from the system");
        error = sr::ErrorCode::OperationFailed;
    }

    switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());
                SRPLG_LOG_INF(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                for (const auto& m : change.node.meta()) {
                    SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Meta %s = %s", m.name().data(), m.valueStr().data());
                }

                const auto value = change.node.asTerm().valueStr();
                const auto domain = value.data();
                SRPLG_LOG_INF(PLUGIN_NAME, "Node value: %s", domain);

                switch (change.operation) {
                    case sysrepo::ChangeOperation::Created:
                    case sysrepo::ChangeOperation::Modified:
                        search_list.createSearchDomain(domain);
                        break;
                    case sysrepo::ChangeOperation::Deleted:
                        search_list.deleteSearchDomain(domain);
                        break;
                    case sysrepo::ChangeOperation::Moved:
                        break;
                }
            }

            // store created changes to the system
            try {
                search_list.storeToSystem();
            } catch (const std::runtime_error& err) {
                SRPLG_LOG_ERR(ietf::sys::PLUGIN_NAME, "Unable to store DNS search domain changes to the system");
                error = sysrepo::ErrorCode::OperationFailed;
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
DnsServerModuleChangeCb::DnsServerModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx) { m_ctx = ctx; }

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

    switch (event) {
        case sysrepo::Event::Change:
            for (auto& change : session.getChanges(subXPath->data())) {
                SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

                SRPLG_LOG_INF(
                    ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

                for (const auto& m : change.node.meta()) {
                    SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Meta %s = %s", m.name().data(), m.valueStr().data());
                }

                switch (change.operation) {
                    case sysrepo::ChangeOperation::Created:
                    case sysrepo::ChangeOperation::Modified:
                        {
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

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
DnsTimeoutModuleChangeCb::DnsTimeoutModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx) { m_ctx = ctx; }

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
DnsAttemptsModuleChangeCb::DnsAttemptsModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx) { m_ctx = ctx; }

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
}

/**
 * DNS module constructor. Allocates each context.
 */
DnsModule::DnsModule()
{
    m_operContext = std::make_shared<DnsOperationalContext>();
    m_changeContext = std::make_shared<DnsModuleChangesContext>();
    m_rpcContext = std::make_shared<DnsRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<srpc::IModuleContext> DnsModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<srpc::IModuleContext> DnsModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<srpc::IModuleContext> DnsModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<srpc::OperationalCallback> DnsModule::getOperationalCallbacks()
{
    return {
        srpc::OperationalCallback { "/ietf-system:system/dns-resolver/search", ietf::sys::sub::oper::DnsSearchOperGetCb(m_operContext) },
        srpc::OperationalCallback { "/ietf-system:system/dns-resolver/server", ietf::sys::sub::oper::DnsServerOperGetCb(m_operContext) },
        srpc::OperationalCallback { "/ietf-system:system/dns-resolver/options", ietf::sys::sub::oper::DnsOptionsOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<srpc::ModuleChangeCallback> DnsModule::getModuleChangeCallbacks()
{
    return {
        srpc::ModuleChangeCallback { "/ietf-system:system/dns-resolver/search", ietf::sys::sub::change::DnsSearchModuleChangeCb(m_changeContext) },
        srpc::ModuleChangeCallback { "/ietf-system:system/dns-resolver/server", ietf::sys::sub::change::DnsServerModuleChangeCb(m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<srpc::RpcCallback> DnsModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* DnsModule::getName() { return "DNS"; }
