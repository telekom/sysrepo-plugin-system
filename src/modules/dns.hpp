#pragma once

#include <core/module.hpp>
#include <core/ip.hpp>

/**
 * Operational context for the DNS module.
 */
class DnsOperationalContext : public IModuleContext { };

/**
 * Module changes context for the DNS module.
 */
class DnsModuleChangesContext : public IModuleContext { };

/**
 * RPC context for the DNS module.
 */
class DnsRpcContext : public IModuleContext { };

namespace ietf::sys::dns {
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

namespace ietf::sys::sub::oper {

}

namespace ietf::sys::sub::change {

}

/**
 * @brief DNS container module.
 */
class DnsModule : public IModule {
public:
    /**
     * DNS module constructor. Allocates each context.
     */
    DnsModule();

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<RpcCallback> getRpcCallbacks() override;

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