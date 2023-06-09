#include "dns.hpp"
#include "core/callbacks.hpp"

#include <core/sdbus.hpp>

// logging
#include <sysrepo.h>

namespace ietf::sys::dns {

// DnsServer::DnsServer(int ifindex, std::string name, ip::Address address, uint16_t port)
//     : m_name { name }
//     , m_address { address }
//     , m_port { port }
//     , m_ifindex { ifindex }
// {
// }

// std::string DnsServer::getName() { return m_name; }

// uint16_t DnsServer::getPort() { return m_port; }

// ip::Address* DnsServer::getAddress() { return &m_address; }

// void DnsServer::setPort(const uint16_t& port) { this->m_port = port; }

// void DnsServer::setAddress(const ip::Address& address) { this->m_address = address; }

// std::string DnsServer::getStringAddress() { return m_address.getStringAddr(); }

// int DnsServer::getIfindex() { return m_ifindex; }

// bool DnsServer::operator==(const DnsServer& other) const
// {
//     return ((this->m_name == other.m_name) && (this->m_address == other.m_address) && (this->m_port == other.m_port)
//         && (this->m_ifindex == other.m_ifindex));
// }

// // DnsSearchServer implementation
// DnsSearchServer::DnsSearchServer()
//     : m_search { false }
// {
//     m_domain.clear();
// }

// DnsSearchServer::DnsSearchServer(int ifindex, std::string domain, bool search)
//     : m_domain { domain }
//     , m_search { search }
//     , m_ifindex { ifindex }
// {
// }

// DnsSearchServer::DnsSearchServer(std::string domain, bool search)
//     : m_domain { domain }
//     , m_search { search }
//     , m_ifindex { SYSTEMD_IFINDEX }
// {
// }

// std::string DnsSearchServer::getDomain() { return m_domain; }

// bool DnsSearchServer::getSearch() { return m_search; }

// int DnsSearchServer::getIfIndex() { return m_ifindex; }

// void DnsSearchServer::setDomain(std::string domain) { this->m_domain = domain; }

// void DnsSearchServer::setSearch(bool search) { this->m_search = search; }

// bool DnsSearchServer::operator==(const DnsSearchServer& other) const
// {
//     return (this->m_domain == other.m_domain) && (this->m_ifindex == other.m_ifindex);
// }

// bool DnsSearchServer::operator!=(const DnsSearchServer& other) const
// {
//     return !((this->m_domain == other.m_domain) && (this->m_ifindex == other.m_ifindex));
// }

// // DnsSearchServerList implementation

// DnsSearchServerList::DnsSearchServerList()
//     : m_ifindex { SYSTEMD_IFINDEX }
// {
// }

// // not recomended, explicit definition of ifindex
// DnsSearchServerList::DnsSearchServerList(int ifindex)
//     : m_ifindex { ifindex }
// {
// }

// bool DnsSearchServerList::addDnsSearchServer(DnsSearchServer srv)
// {
//     for (DnsSearchServer& server : m_servers) {
//         if (server == srv) {
//             // allready exists or wrong ifindex
//             if (server.getSearch() != srv.getSearch()) {
//                 server.setSearch(srv.getSearch());
//                 // just modified, dont push to vector
//                 return true;
//             }
//             return false;
//         }
//     }

//     m_servers.push_back(srv);
//     return true;
// }

// std::vector<DnsSearchServer> DnsSearchServerList::getAllServers() { return m_servers; }

// std::optional<DnsSearchServer> DnsSearchServerList::findDnsSearchServer(const DnsSearchServer& server)
// {

//     for (DnsSearchServer& srv : m_servers) {
//         if (srv == server) {
//             return server;
//         }
//     }

//     return std::nullopt;
// }

// bool DnsSearchServerList::removeDnsSearchServer(const DnsSearchServer& server)
// {
//     std::vector<DnsSearchServer>::iterator it = m_servers.begin();
//     // safe aproach to erase while iterating
//     while (it != m_servers.end()) {

//         if (*it == server) {
//             it = m_servers.erase(it);
//             return true;
//         } else {
//             it++;
//         }
//     }

//     return false;
// }

// bool DnsSearchServerList::compareDnsSearchServer(const DnsSearchServer& s1, const DnsSearchServer& s2) { return (s1 == s2); }

// bool DnsSearchServerList::exportListToSdBus()
// {

//     bool error = false;

//     std::vector<sdbus::Struct<std::string, bool>> sdbusData;

//     for (auto& server : m_servers) {
//         sdbusData.push_back(sdbus::Struct<std::string, bool>(sdbus::make_struct(server.getDomain(), server.getSearch())));
//     }

//     const char* destinationName = "org.freedesktop.resolve1";
//     const char* objectPath = "/org/freedesktop/resolve1";
//     const char* interfaceName = "org.freedesktop.resolve1.Manager";

//     try {
//         auto proxy = sdbus::createProxy(destinationName, objectPath);
//         proxy->callMethod("SetLinkDomains").onInterface(interfaceName).withArguments(m_ifindex, sdbusData);
//     } catch (sdbus::Error& e) {
//         SRPLG_LOG_ERR("%s", e.getMessage().c_str());
//         error = true;
//     }

//     // clear container after succsess
//     if (!error) {
//         m_servers.clear();
//     }

//     return error;
// }

// bool DnsSearchServerList::importListFromSdBus()
// {

//     bool error = false;

//     std::vector<sdbus::Struct<int32_t, std::string, bool>> sdbusData;
//     const char* destinationName = "org.freedesktop.resolve1";
//     const char* objectPath = "/org/freedesktop/resolve1";
//     const char* interfaceName = "org.freedesktop.resolve1.Manager";
//     sdbus::Variant v;
//     try {
//         auto proxy = sdbus::createProxy(destinationName, objectPath);
//         v = proxy->getProperty("Domains").onInterface(interfaceName);
//     } catch (sdbus::Error& e) {
//         SRPLG_LOG_ERR("%s", e.getMessage().c_str());
//         error = true;
//     }
//     sdbusData = v.get<std::vector<sdbus::Struct<int32_t, std::string, bool>>>();
//     m_servers.clear();
//     for (auto& vc : sdbusData) {
//         // filter by ifindex
//         int ifindex = vc.get<0>();
//         if (ifindex == m_ifindex) {
//             m_servers.push_back(
//                 // store the ifindex ,domain, search
//                 DnsSearchServer(ifindex, vc.get<1>(), vc.get<2>()));
//         }
//     }

//     return error;
// }

// // end of DnsSearchServerList initialization

// // DnsServerList initialization

// DnsServerList::DnsServerList()
//     : m_ifindex { SYSTEMD_IFINDEX }
// {
// }
// DnsServerList::DnsServerList(int ifindex)
//     : m_ifindex { ifindex }
// {
// }

// bool DnsServerList::importListFromSdBus()
// {

//     bool error = false;

//     std::vector<sdbus::Struct<int, int, std::vector<uint8_t>, uint16_t, std::string>> sdbusData;
//     const char* destinationName = "org.freedesktop.resolve1";
//     const char* objectPath = "/org/freedesktop/resolve1";
//     const char* interfaceName = "org.freedesktop.resolve1.Manager";
//     sdbus::Variant v;

//     try {
//         auto proxy = sdbus::createProxy(destinationName, objectPath);
//         v = proxy->getProperty("DNSEx").onInterface(interfaceName);
//     } catch (sdbus::Error& e) {
//         SRPLG_LOG_ERR("%s", e.getMessage().c_str());
//         error = true;
//     }
//     // ifindex, version (ipv4=2 ipv6=10), byte_array, port, name
//     sdbusData = v.get<std::vector<sdbus::Struct<int, int, std::vector<uint8_t>, uint16_t, std::string>>>();
//     m_servers.clear();

//     for (auto& vc : sdbusData) {

//         int ifindex = vc.get<0>();
//         if (ifindex == m_ifindex) {

//             std::shared_ptr<ip::Address> addr;

//             int version = vc.get<1>();
//             int port = vc.get<3>();

//             if (port == 0) {
//                 port = 53;
//             }

//             if (version == 2) {
//                 addr = std::make_shared<ip::Ipv4Address>(vc.get<2>());
//             } else if (version == 10) {
//                 addr = std::make_shared<ip::Ipv6Address>(vc.get<2>());
//             }

//             // Address addr(vc.get<2>());
//             m_servers.push_back(DnsServer(ifindex, vc.get<4>(), *addr, port));
//         }
//     }

//     return error;
// }

// bool DnsServerList::exportListToSdBus()
// {
//     bool error = false;

//     std::vector<sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>> sdbusData;

//     for (auto& server : m_servers) {
//         sdbusData.push_back(sdbus::Struct<int, std::vector<uint8_t>, uint16_t, std::string>(
//             sdbus::make_struct(server.getAddress()->getVersion(), server.getAddress()->byteVector(), server.getPort(), server.getName())));
//     }

//     const char* destinationName = "org.freedesktop.resolve1";
//     const char* objectPath = "/org/freedesktop/resolve1";
//     const char* interfaceName = "org.freedesktop.resolve1.Manager";

//     try {
//         auto proxy = sdbus::createProxy(destinationName, objectPath);
//         proxy->callMethod("SetLinkDNSEx").onInterface(interfaceName).withArguments(m_ifindex, sdbusData);
//     } catch (sdbus::Error& e) {
//         SRPLG_LOG_ERR("%s", e.getMessage().c_str());
//         error = true;
//     }

//     // clear container after succsess
//     if (!error) {
//         m_servers.clear();
//     }

//     return error;
// }

// int DnsServerList::getIfIndex() { return m_ifindex; }

// bool DnsServerList::addDnsServer(DnsServer srv)
// {
//     for (DnsServer& server : m_servers) {
//         if (server.getName().compare(srv.getName()) == 0) {
//             // the name is the key in yang
//             // allready exists, return false
//             return false;
//         }
//     }

//     m_servers.push_back(srv);
//     return true;
// }

// std::vector<DnsServer> DnsServerList::getDnsServerVector() { return m_servers; }

// bool DnsServerList::removeDnsServer(const DnsServer& server)
// {
//     std::vector<DnsServer>::iterator it = m_servers.begin();
//     // safe aproach to erase while iterating
//     while (it != m_servers.end()) {

//         if (*it == server) {
//             it = m_servers.erase(it);
//             return true;
//         } else {
//             it++;
//         }
//     }

//     return false;
// }

// bool DnsServerList::modifyDnsServer(DnsServer server)
// {

//     for (auto& srv : m_servers) { // name is the key in yang model, no duplicates
//         if (srv.getName().compare(server.getName()) == 0) {

//             if (srv.getAddress()->getVersion() != server.getAddress()->getVersion()) {
//                 throw std::runtime_error("Server has diferent IPVersion, cannot modify!");
//             }

//             srv.setAddress(*server.getAddress());
//             srv.setPort(server.getPort());
//             return true;
//         }
//     }

//     return false;
// }

// // end of DnsServerList init

// // function to take from the changed node, return DnsServer
// std::optional<DnsServer> getServerFromChangedNode(const libyang::DataNode& changedNode)
// {

//     libyang::DataNode recursiveNode(changedNode);
//     libyang::SchemaNode schema = recursiveNode.schema();
//     std::unordered_map<std::string, std::string> map;

//     // starting from the lowest point address or port
//     if (schema.name().compare("address") == 0 || schema.name().compare("port") == 0) {

//         // set the node pointer to server node
//         while (schema.name().compare("server") != 0) {
//             recursiveNode = recursiveNode.parent().value();
//             schema = recursiveNode.schema();
//         }

//         for (libyang::DataNode node : recursiveNode.childrenDfs()) {

//             // jump the first node, cause its server
//             if (node.schema().name().compare("server") == 0)
//                 continue;

//             if (node.isTerm()) {
//                 {
//                     std::string key = node.schema().name().data();
//                     std::string value = node.asTerm().valueStr().data();
//                     map.insert(std::make_pair(key, value));
//                 }

//             } else {

//                 {
//                     std::string value = node.schema().name().data();
//                     std::string key = "transport";
//                     map.insert(std::make_pair(key, value));
//                 }
//             }
//         }

//         uint16_t port = 53;
//         std::shared_ptr<ip::Address> addr = nullptr;

//         if (map.find("port") != map.end()) {
//             port = stoi(map.at("port"));
//         }

//         std::string adr = map["address"];
//         struct in_addr ip_addr;
//         struct in6_addr ip_addr_6;
//         // int ip_version = getAddressIPVersion(adr);

//         if (inet_pton(AF_INET, adr.c_str(), &ip_addr) == 1) {

//             addr = std::make_shared<ip::Ipv4Address>(adr);

//         } else if (inet_pton(AF_INET6, adr.c_str(), &ip_addr_6) == 1) {

//             addr = std::make_shared<ip::Ipv6Address>(adr);

//         } else {
//             throw std::runtime_error("Unknown address format!");
//         }

//         DnsServer server(SYSTEMD_IFINDEX, map["name"], *addr, port);
//         return server;
//     }

//     return std::nullopt;
// }

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

    // dns::DnsSearchServerList dnsSearchServers;
    // // get all from bus

    // switch (event) {
    // case sysrepo::Event::Change:

    //     if (dnsSearchServers.importListFromSdBus()) {
    //         SRPLG_LOG_ERR(PLUGIN_NAME, "%s", "sd bus import failed!");
    //         return sr::ErrorCode::OperationFailed;
    //     }

    //     // if the call succseeded - continue
    //     for (auto& change : session.getChanges(subXPath->data())) {
    //         switch (change.operation) {
    //         case sysrepo::ChangeOperation::Created:
    //         case sysrepo::ChangeOperation::Modified: {

    //             // get the changed values in node
    //             auto value = change.node.asTerm().value();
    //             auto domain = std::get<std::string>(value);

    //             // no value provided for search param - default true? or maybe another param?
    //             sys::dns::DnsSearchServer server(domain, true);
    //             dnsSearchServers.addDnsSearchServer(server);

    //             break;
    //         }
    //         case sysrepo::ChangeOperation::Deleted: {
    //             // modification:
    //             // first it goes to delete event, then create
    //             // take deleted from here

    //             auto deletedValue = change.node.asTerm().value();
    //             auto deletedDomain = std::get<std::string>(deletedValue);

    //             sys::dns::DnsSearchServer deleted(deletedDomain, true);

    //             dnsSearchServers.removeDnsSearchServer(deleted);

    //             break;
    //         }

    //         case sysrepo::ChangeOperation::Moved:
    //             break;
    //         }
    //     }

    //     // process finished -> export
    //     try {
    //         dnsSearchServers.exportListToSdBus();
    //     } catch (sdbus::Error& e) {
    //         SRPLG_LOG_ERR(PLUGIN_NAME, "%s", e.getMessage().c_str());
    //         return sr::ErrorCode::OperationFailed;
    //     }

    //     break;
    // default:
    //     break;
    // }

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

    // dns::DnsServerList dnsList;
    // switch (event) {
    // case sysrepo::Event::Change:

    //     // first take all from sdbus
    //     if (dnsList.importListFromSdBus()) {
    //         SRPLG_LOG_ERR(PLUGIN_NAME, "%s", "sd bus import failed!");
    //         return sr::ErrorCode::OperationFailed;
    //     }

    //     for (sysrepo::Change change : session.getChanges(subXPath->data())) {
    //         switch (change.operation) {
    //         case sysrepo::ChangeOperation::Created: {

    //             std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

    //             if (server == std::nullopt) {
    //                 error = sr::ErrorCode::OperationFailed;

    //             } else {
    //                 dnsList.addDnsServer(server.value());
    //                 error = sr::ErrorCode::Ok;
    //             }
    //             break;
    //         }

    //         case sysrepo::ChangeOperation::Modified: {

    //             std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

    //             if (server == std::nullopt) {
    //                 error = sr::ErrorCode::OperationFailed;

    //             } else {
    //                 // modify here
    //                 try {
    //                     dnsList.modifyDnsServer(server.value());
    //                 } catch (std::exception(&e)) {
    //                     SRPLG_LOG_ERR(PLUGIN_NAME, "%s", e.what());
    //                     return sr::ErrorCode::OperationFailed;
    //                 };

    //                 error = sr::ErrorCode::Ok;
    //             }
    //             break;
    //         }

    //         case sysrepo::ChangeOperation::Deleted: {

    //             std::optional<dns::DnsServer> server = dns::getServerFromChangedNode(change.node);

    //             if (server == std::nullopt) {
    //                 error = sr::ErrorCode::OperationFailed;

    //             } else {
    //                 dnsList.removeDnsServer(server.value());
    //                 error = sr::ErrorCode::Ok;
    //             }

    //             // // deleted code here
    //             break;
    //         }

    //         default:
    //             break;
    //         }
    //     }
    //     if (dnsList.exportListToSdBus() == true) {
    //         return sr::ErrorCode::OperationFailed;
    //     }
    //     break;

    // default:
    //     break;
    // }

    return error;
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
std::shared_ptr<IModuleContext> DnsModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<IModuleContext> DnsModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<IModuleContext> DnsModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<OperationalCallback> DnsModule::getOperationalCallbacks()
{
    return {
        OperationalCallback { "/ietf-system:system/dns-resolver/search", ietf::sys::sub::oper::DnsSearchOperGetCb(m_operContext) },
        OperationalCallback { "/ietf-system:system/dns-resolver/server", ietf::sys::sub::oper::DnsServerOperGetCb(m_operContext) },
        OperationalCallback { "/ietf-system:system/dns-resolver/options", ietf::sys::sub::oper::DnsOptionsOperGetCb(m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<ModuleChangeCallback> DnsModule::getModuleChangeCallbacks()
{
    return {
        ModuleChangeCallback { "/ietf-system:system/dns-resolver/search", ietf::sys::sub::change::DnsSearchModuleChangeCb(m_changeContext) },
        ModuleChangeCallback { "/ietf-system:system/dns-resolver/server//*", ietf::sys::sub::change::DnsServerModuleChangeCb(m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<RpcCallback> DnsModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* DnsModule::getName() { return "DNS"; }