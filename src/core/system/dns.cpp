#include "dns.hpp"
#include "sdbus-c++/Message.h"

#include <sdbus-c++/sdbus-c++.h>

namespace ietf::sys::dns {

/**
 * @brief Default constructor.
 */
DnsServer::DnsServer()
    : Port(53)
{
}

/**
 * @brief Get the list of DNS search domains.
 * @return List of DNS search domains.
 */
DnsSearchList getSearchList()
{
    DnsSearchList search_list;

    return search_list;
}

}