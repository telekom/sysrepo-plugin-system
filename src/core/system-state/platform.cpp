#include "platform.hpp"

// Platform information
#include <stdexcept>
#include <memory>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys::state {

/**
 * @brief Get platform information.
 *
 * @return Platform information.
 */
PlatformInfo getPlatformInfo()
{
    struct utsname uname_data = { 0 };

    if (uname(&uname_data) < 0) {
        throw std::runtime_error("Failed to get platform information.");
    }

    return {
        .OsName = uname_data.sysname,
        .OsRelease = uname_data.release,
        .OsVersion = uname_data.version,
        .Machine = uname_data.machine,
    };
}

}