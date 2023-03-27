#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <optional>

namespace ietf::sys {

/**
 * @brief Authorized key helper struct.
 */
struct AuthorizedKey {
    std::string Name; ///< Key name.
    std::string Algorithm; ///< Key algorithm.
    std::string Data; ///< Key data.
};

using AuthorizedKeyList = std::vector<AuthorizedKey>;

/**
 * @brief Local user helper struct.
 */
struct LocalUser {
    std::string Name; ///< User name.
    std::optional<std::string> Password; ///< User password hash.
    std::optional<AuthorizedKeyList> AuthorizedKeys; ///< User authorized keys.
};

using LocalUserList = std::vector<LocalUser>;

// other type aliases
using Hostname = std::string;
using TimezoneName = std::string;

}