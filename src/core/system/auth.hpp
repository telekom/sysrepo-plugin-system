#pragma once

#include <string>
#include <vector>
#include <optional>

namespace ietf::sys::auth {

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