#pragma once

#include <string>

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

}