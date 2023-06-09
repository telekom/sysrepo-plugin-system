#pragma once

#include <string>
#include <vector>
#include <optional>

namespace ietf::sys::auth {

/**
 * @brief DNS search type.
 */
using DnsSearch = std::string;

/**
 * @brief DNS search list type.
 */
using DnsSearchList = std::vector<DnsSearch>;

/**
 * @brief DNS server type.
 */
struct DnsServer {
    std::string Name; ///< Server name.
    std::string Address; ///< Server address.
    std::int16_t Port; ///< Server port.

    /**
     * @brief Default constructor - sets the default DNS port.
     */
    DnsServer()
        : Port(53)
    {
    }
};

using DnsServerList = std::vector<DnsServer>;

/**
 * @brief DNS options helper struct.
 */
struct DnsOptions {
    uint8_t Timeout; ///< Timeout.
    uint8_t Attempts; ///< Attempts.
};

/**
 * @brief Authorized key helper struct.
 */
struct AuthorizedKey {
    std::string Name; ///< Key name.
    std::string Algorithm; ///< Key algorithm.
    std::string Data; ///< Key data.
};

/**
 * @brief Authorized key list type alias.
 */
using AuthorizedKeyList = std::vector<AuthorizedKey>;

/**
 * @brief Local user helper struct.
 */
struct LocalUser {
    std::string Name; ///< User name.
    std::optional<std::string> Password; ///< User password hash.
    std::optional<AuthorizedKeyList> AuthorizedKeys; ///< User authorized keys.
};

/**
 * @brief Local user list type alias.
 */
using LocalUserList = std::vector<LocalUser>;

/**
 * @brief Get system local users.
 *
 * @return Local users on the system.
 */
LocalUserList getLocalUserList();

/**
 * @brief Get local user authorized keys.
 *
 * @param username Username.
 *
 * @return Authorized keys.
 */
AuthorizedKeyList getAuthorizedKeyList(const std::string& username);
}