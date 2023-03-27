#pragma once

#include <string>

namespace ietf::sys::state {
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
 * @brief Get platform information.
 *
 * @return Platform information.
 */
PlatformInfo getPlatformInfo();
}