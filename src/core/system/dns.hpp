#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <sdbus-c++/sdbus-c++.h>
#include <sysrepo.h>
#include <libyang-cpp/DataNode.hpp>
#include <libyang-cpp/Collection.hpp>
#include <variant>

namespace ietf::sys::dns {

class Address {
public:
    Address(std::string address);
    Address(std::vector<uint8_t>);

    int getVersion();
    std::string getStringAddr();

    std::vector<uint8_t> byteVector();

    bool operator==(const Address& other) const;

private:
    uint8_t BYTE_SIZE;
    std::string address;

    int version;

    union dns {
        uint8_t ipv6_address[16];
        uint8_t ipv4_address[4];
    } dns;
};

/**
 * @brief DNS server.
 */
class DnsServer {
private:
    std::string Name; ///< Arbitrary name of the DNS server.
    Address address; ///< Address of the DNS server.
    std::uint16_t Port; ///< Port of the DNS server.
    int m_ifindex;

public:
    DnsServer(int ifindex, std::string name, Address address, std::uint16_t port);
    // void setName(const std::string name);
    void setAddress(const Address& address);
    void setPort(const uint16_t& port);
    std::string getName();
    std::string getStringAddress();
    dns::Address getAddress();
    std::uint16_t getPort();
    int getIfindex();

    bool operator==(const DnsServer& other) const;
    bool operator!=(const DnsServer& other) const;
};

/**
 * @brief DnsSearchServer class
 */
class DnsSearchServer {

private:
    std::string Domain;
    bool Search;
    int m_ifindex;

public:
    /**
     * @brief Default constructor
     */
    DnsSearchServer();

    /**
     * @brief Constructor with all parameters
     */
    DnsSearchServer(std::string domain, bool search);

    /**
     * @brief Return the ifindes
     */
    int getIfIndex();

    // /**
    //  * @brief set the ifindex
    //  */
    // void setIfIndex(const int& ifindex);

    /**
     * @brief Set DnsSearchServer Domain
     */
    void setDomain(std::string domain);

    /**
     * @brief Set DnsSearchServer search
     */
    void setSearch(bool search);

    /**
     * @brief Get DnsSearchServer Domain
     */
    std::string getDomain();

    /**
     * @brief Get DnsSearchServer Search
     */
    bool getSearch();

    /**
     * @brief Overloaded == operator
     * @details It compares them by the Domain
     */
    bool operator==(const DnsSearchServer& other) const;

    /**
     * @brief Overloaded != operator
     * @details It compares them by the Domain
     */
    bool operator!=(const DnsSearchServer& other) const;
};

/**
 * @brief DnsSearchServerList class
 * @details Static class for storring DnsSearchServer objects
 */

class DnsSearchServerList {

private:
    int m_ifindex;
    std::vector<DnsSearchServer> servers;

public:
    DnsSearchServerList();
    DnsSearchServerList(int ifindex);

    // void setIfIndex(const int& ifindex);
    int getIfIndex();
    bool addDnsSearchServer(DnsSearchServer srv);
    std::vector<DnsSearchServer> getAllServers();

    /**
     * @brief Find DnsSearchServer by DnsSearchServer
     * @return std::optional<DnsSearchServer>
     * @details Returns std::nullopt if object is not found
     */
    std::optional<DnsSearchServer> findDnsSearchServer(const DnsSearchServer& server);

    /**
     * @brief Remove DnsSearchServer
     * @return true if removed, false otherwise
     */
    bool removeDnsSearchServer(const DnsSearchServer& server);

    /**
     * @brief Compare DnsSearchServer
     * @param s1,s2 Compares 2 DnsSearchServers
     * @return true if are the same, false otherwise
     */
    bool compareDnsSearchServer(const DnsSearchServer& s1, const DnsSearchServer& s2);

    /**
     * @brief Call sdbus command with current object
     */
    bool exportListToSdBus();

    /**
     * @warning it overwrites all existing dns search servers
     * @brief Get all Domains from sdbus
     */
    bool importListFromSdBus();
};

class DnsServerList {
private:
    int m_ifindex;
    std::vector<DnsServer> servers;

public:
    DnsServerList();
    DnsServerList(int ifindex);

    int getIfIndex();
    bool addDnsServer(DnsServer srv);

    bool removeDnsServer(const DnsServer& server);

    bool exportListToSdBus();

    bool importListFromSdBus();

    bool modifyDnsServer(DnsServer server);

    std::vector<DnsServer> getDnsServerVector();
};

std::optional<DnsServer> getServerFromChangedNode(const libyang::DataNode& changedNode);

}