#pragma once

#include <string>

namespace ietf::sys::state {

/**
 * @brief Clock information helper struct.
 */
struct ClockInfo {
    std::string BootDatetime; ///< Boot datetime.
    std::string CurrentDatetime; ///< Current datetime.
};

/**
 * @brief Get clock information.
 *
 * @return Clock information.
 */
ClockInfo getClockInfo();

}