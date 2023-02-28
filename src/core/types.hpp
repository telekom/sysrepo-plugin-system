#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <optional>

namespace ietf::sys {

/**
 * @brief Platform information helper struct.
 */
struct PlatformInfo {
    std::string OsName; ///< OS name.
    std::string OsRelease; ///< OS release.
    std::string OsVersion; ///< OS version.
    std::string Machine; ///< Machine.
};

/**
 * @brief Clock information helper struct.
 */
struct ClockInfo {
    std::string BootDatetime; ///< Boot datetime.
    std::string CurrentDatetime; ///< Current datetime.
};

/**
 * @brief Authorized key helper struct.
 */
struct AuthorizedKey {
    std::string Name; ///< Key name.
    std::string Algorithm; ///< Key algorithm.
    std::vector<std::byte> Data; ///< Key data.
};

using AuthorizedKeyList = std::vector<AuthorizedKey>;

/**
 * @brief Local user helper struct.
 */
struct LocalUser {
    std::string Name; ///< User name.
    std::string Password; ///< User password hash.
    std::optional<AuthorizedKeyList> AuthorizedKeys; ///< User authorized keys.
};

using LocalUserList = std::vector<LocalUser>;

// other type aliases
using Hostname = std::string;
using TimezoneName = std::string;

}