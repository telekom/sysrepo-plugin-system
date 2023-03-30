#pragma once

#include <string>
#include <vector>
#include <cstdint>

namespace ietf::sys::dns {

/**
 * @brief DNS server.
 */
struct DnsServer {
    std::string Name; ///< Arbitrary name of the DNS server.
    std::string Address; ///< Address of the DNS server.
    std::uint16_t Port; ///< Port of the DNS server.

    using List = std::vector<DnsServer>;

    /**
     * @brief Default constructor.
     */
    DnsServer();

    /**
     * @brief Get the list of DNS servers.
     * @return List of DNS servers.
     */
    static DnsServer::List getServers();
};

/**
 * @brief DNS search element type alias.
 */
using DnsSearch = std::string;

/**
 * @brief DNS search list type alias.
 */
using DnsSearchList = std::vector<DnsSearch>;

/**
 * @brief Get the list of DNS search domains.
 * @return List of DNS search domains.
 */
DnsSearchList getSearchList();

}