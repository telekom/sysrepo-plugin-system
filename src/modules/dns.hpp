#pragma once

#include <cstdint>
#include <srpcpp/module.hpp>
#include <core/ip.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include "core/context.hpp"
#include "core/sdbus.hpp"

// helpers
namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::sys::dns {

/**
 * @brief DNS server helper struct.
 */
struct DnsServer {
    int32_t InterfaceIndex; ///< Interface index used for the DNS server.
    std::unique_ptr<ip::IAddress> Address; ///< IP address of the server.
    uint16_t Port; ///< Port used for the server. Defaults to 53.
    std::string Name; ///< Server Name Indication.

    /**
     * @brief Default constructor.
     */
    DnsServer();

    /**
     * @brief Set the IP address of the server.
     *
     * @param address IP address (IPv4 or IPv6).
     */
    void setAddress(const std::string& address);

    /**
     * @brief Set the port of the server.
     *
     * @param port Port to set.
     */
    void setPort(std::optional<uint16_t> port);
};

/**
 * @brief DNS search helper struct.
 */
struct DnsSearch {
    int InterfaceIndex; ///< Interface index of the search element. 0 used for global configuration.
    std::string Domain; ///< Domain of the search element.
    bool Search; ///< Boolean value indicating wether the value is used for routing (true) or for both routing and searching (false).
};

/**
 * @brief DNS server list class used for loading and storing a list of DNS servers.
 */
class DnsServerList : public SdBus<std::vector<sdbus::Struct<int32_t, int32_t, std::vector<uint8_t>, uint16_t, std::string>>, int32_t,
                          std::vector<sdbus::Struct<int32_t, std::vector<uint8_t>, uint16_t, std::string>>> {
public:
    /**
     * @brief Default constructor.
     */
    DnsServerList();

    /**
     * @brief Loads the list of DNS servers found currently on the system.
     */
    void loadFromSystem();

    /**
     * @brief Stores the list of DNS servers in the class to the system.
     */
    void storeToSystem();

    /**
     * @brief Create a new server and add it to the list.
     *
     * @param name Name of the DNS server.
     * @param address IP address of the DNS server.
     * @param port Optional port value of the DNS server. If no value provided, 53 is used.
     */
    void createServer(const std::string& name, const std::string& address, std::optional<uint16_t> port);

    /**
     * @brief Change the IP address of the given server with the provided name.
     *
     * @param name Name of the server to change.
     * @param address New address to set.
     */
    void changeServerAddress(const std::string& name, const std::string& address);

    /**
     * @brief Change the port of the given server with the provided name.
     *
     * @param name Name of the server to change.
     * @param port New port to set.
     */
    void changeServerPort(const std::string& name, const uint16_t port);

    /**
     * @brief Delete server from the list.
     *
     * @param name Name of the DNS server.
     */
    void deleteServer(const std::string& name);

    /**
     * @brief Get iterator to the beginning.
     */
    auto begin() { return m_servers.begin(); }

    /**
     * @brief Get iterator to the end.
     */
    auto end() { return m_servers.end(); }

private:
    /**
     * @brief Helper function for finding DNS server by the provided name.
     *
     * @param name Name to use for search.
     *
     * @return Iterator pointing to the DNS server with the provided name.
     */
    std::optional<std::list<DnsServer>::iterator> m_findServer(const std::string& name);

    int m_ifindex; ///< Interface index used for this list.
    std::list<DnsServer> m_servers; ///< List of DNS servers.
};

/**
 * @breif DNS search list class used for loading and storing a list of DNS search domains.
 */
class DnsSearchList : public SdBus<std::vector<sdbus::Struct<int32_t, std::string, bool>>, int32_t, std::vector<sdbus::Struct<std::string, bool>>> {
public:
    /**
     * @brief Default constructor.
     */
    DnsSearchList();

    /**
     * @brief Loads the list of DNS servers found currently on the system.
     */
    void loadFromSystem();

    /**
     * @brief Add new search domain to the list.
     *
     * @param domain Search domain to create.
     */
    void createSearchDomain(const std::string& domain);

    /**
     * @brief Delete search domain from the list.
     *
     * @param domain Search domain to remove.
     */
    void deleteSearchDomain(const std::string& domain);

    /**
     * @brief Stores the list of DNS servers in the class to the system.
     */
    void storeToSystem();

    /**
     * @brief Get iterator to the beginning.
     */
    auto begin() { return m_search.begin(); }

    /**
     * @brief Get iterator to the end.
     */
    auto end() { return m_search.end(); }

private:
    int m_ifindex; ///< Interface index used for this list.
    std::list<DnsSearch> m_search; ///< List of DNS search domains.
};

// /**
//  * @brief DNS server.
//  */
// class DnsServer {
// private:
//     std::string m_name; ///< Arbitrary name of the DNS server.
//     ip::Address m_address; ///< Address of the DNS server.
//     std::uint16_t m_port; ///< Port of the DNS server.
//     int m_ifindex;

// public:
//     DnsServer(int ifindex, std::string name, ip::Address address, std::uint16_t port);
//     // void setName(const std::string name);
//     void setAddress(const ip::Address& address);
//     void setPort(const uint16_t& port);
//     std::string getName();
//     std::string getStringAddress();
//     ip::Address* getAddress();
//     std::uint16_t getPort();
//     int getIfindex();

//     bool operator==(const DnsServer& other) const;
//     bool operator!=(const DnsServer& other) const;
// };

// /**
//  * @brief DnsSearchServer class
//  */
// class DnsSearchServer {

// private:
//     std::string m_domain;
//     bool m_search;
//     int m_ifindex;

// public:
//     /**
//      * @brief Default constructor
//      */
//     DnsSearchServer();

//     /**
//      * @brief Constructor with all parameters
//      */
//     DnsSearchServer(int ifindex, std::string domain, bool search);
//     DnsSearchServer(std::string domain, bool search);

//     /**
//      * @brief Return the ifindes
//      */
//     int getIfIndex();

//     // /**
//     //  * @brief set the ifindex
//     //  */
//     // void setIfIndex(const int& ifindex);

//     /**
//      * @brief Set DnsSearchServer Domain
//      */
//     void setDomain(std::string domain);

//     /**
//      * @brief Set DnsSearchServer search
//      */
//     void setSearch(bool search);

//     /**
//      * @brief Get DnsSearchServer Domain
//      */
//     std::string getDomain();

//     /**
//      * @brief Get DnsSearchServer Search
//      */
//     bool getSearch();

//     /**
//      * @brief Overloaded == operator
//      * @details It compares them by the Domain
//      */
//     bool operator==(const DnsSearchServer& other) const;

//     /**
//      * @brief Overloaded != operator
//      * @details It compares them by the Domain
//      */
//     bool operator!=(const DnsSearchServer& other) const;
// };

// /**
//  * @brief DnsSearchServerList class
//  * @details Static class for storring DnsSearchServer objects
//  */

// class DnsSearchServerList {

// private:
//     int m_ifindex;
//     std::vector<DnsSearchServer> m_servers;

// public:
//     DnsSearchServerList();
//     DnsSearchServerList(int ifindex);

//     // void setIfIndex(const int& ifindex);
//     int getIfIndex();
//     bool addDnsSearchServer(DnsSearchServer srv);
//     std::vector<DnsSearchServer> getAllServers();

//     /**
//      * @brief Find DnsSearchServer by DnsSearchServer
//      * @return std::optional<DnsSearchServer>
//      * @details Returns std::nullopt if object is not found
//      */
//     std::optional<DnsSearchServer> findDnsSearchServer(const DnsSearchServer& server);

//     /**
//      * @brief Remove DnsSearchServer
//      * @return true if removed, false otherwise
//      */
//     bool removeDnsSearchServer(const DnsSearchServer& server);

//     /**
//      * @brief Compare DnsSearchServer
//      * @param s1,s2 Compares 2 DnsSearchServers
//      * @return true if are the same, false otherwise
//      */
//     bool compareDnsSearchServer(const DnsSearchServer& s1, const DnsSearchServer& s2);

//     /**
//      * @brief Call sdbus command with current object
//      */
//     bool exportListToSdBus();

//     /**
//      * @warning it overwrites all existing dns search servers
//      * @brief Get all Domains from sdbus
//      */
//     bool importListFromSdBus();
// };

// class DnsServerList {
// private:
//     int m_ifindex;
//     std::vector<DnsServer> m_servers;

// public:
//     DnsServerList();
//     DnsServerList(int ifindex);

//     int getIfIndex();
//     bool addDnsServer(DnsServer srv);

//     bool removeDnsServer(const DnsServer& server);

//     bool exportListToSdBus();

//     bool importListFromSdBus();

//     bool modifyDnsServer(DnsServer server);

//     std::vector<DnsServer> getDnsServerVector();
// };

// std::optional<DnsServer> getServerFromChangedNode(const libyang::DataNode& changedNode);
}

/**
 * Operational context for the DNS module.
 */
class DnsOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the DNS module.
 */
class DnsModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the DNS module.
 */
class DnsRpcContext : public srpc::IModuleContext { };

namespace ietf::sys::sub::oper {
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
    DnsSearchOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsServerNameOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsServerUdpAndTcpAddressOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsServerUdpAndTcpPortOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsServerUdpAndTcpOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsServerOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsOptionsTimeoutOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsOptionsAttemptsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsOptionsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
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
    DnsOperGetCb(std::shared_ptr<DnsOperationalContext> ctx);

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
    std::shared_ptr<DnsOperationalContext> m_ctx;
};

}

namespace ietf::sys::sub::change {
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
    DnsSearchModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx);

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
    std::shared_ptr<DnsModuleChangesContext> m_ctx;
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
    DnsServerModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx);

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
    std::shared_ptr<DnsModuleChangesContext> m_ctx;
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
    DnsTimeoutModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx);

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
    std::shared_ptr<DnsModuleChangesContext> m_ctx;
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
    DnsAttemptsModuleChangeCb(std::shared_ptr<DnsModuleChangesContext> ctx);

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
    std::shared_ptr<DnsModuleChangesContext> m_ctx;
};
}

/**
 * @brief Checker used to check if ietf-system/system/dns-resolver/server values are contained on the system.
 */
class DnsServerValueChecker : public srpc::DatastoreValuesChecker<ietf::sys::PluginContext> {
public:
    /**
     * @brief Default constructor.
     */
    DnsServerValueChecker(ietf::sys::PluginContext& plugin_ctx);

    /**
     * @brief Check for the datastore values on the system.
     *
     * @param session Sysrepo session used for retreiving datastore values.
     *
     * @return Enum describing the output of values comparison.
     */
    virtual srpc::DatastoreValuesCheckStatus checkValues(sysrepo::Session& session) override;

    /**
     * @brief Get the paths which the checker is assigned for.
     *
     * @return Checker paths.
     */
    virtual std::list<std::string> getPaths() override
    {
        return {
            "/ietf-system:system/dns-resolver/server",
        };
    }
};

/**
 * @brief Checker used to check if ietf-system/system/dns-resolver/search values are contained on the system.
 */
class DnsSearchValueChecker : public srpc::DatastoreValuesChecker<ietf::sys::PluginContext> {
public:
    /**
     * @brief Default constructor.
     */
    DnsSearchValueChecker(ietf::sys::PluginContext& plugin_ctx);

    /**
     * @brief Check for the datastore values on the system.
     *
     * @param session Sysrepo session used for retreiving datastore values.
     *
     * @return Enum describing the output of values comparison.
     */
    virtual srpc::DatastoreValuesCheckStatus checkValues(sysrepo::Session& session) override;

    /**
     * @brief Get the paths which the checker is assigned for.
     *
     * @return Checker paths.
     */
    virtual std::list<std::string> getPaths() override
    {
        return {
            "/ietf-system:system/dns-resolver/search",
        };
    }
};

/**
 * @brief DNS container module.
 */
class DnsModule : public srpc::IModule<ietf::sys::PluginContext> {
public:
    /**
     * DNS module constructor. Allocates each context.
     */
    DnsModule(ietf::sys::PluginContext& plugin_ctx);

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
     * Dns module destructor.
     */
    ~DnsModule() { }

private:
    std::shared_ptr<DnsOperationalContext> m_operContext;
    std::shared_ptr<DnsModuleChangesContext> m_changeContext;
    std::shared_ptr<DnsRpcContext> m_rpcContext;
};
