#include "clock.hpp"

// system() API
#include <stdexcept>

#include <sys/sysinfo.h>
#include <sys/utsname.h>

namespace ietf::sys::state {
/**
 * @brief Get clock information.
 *
 * @return Clock information.
 */
ClockInfo getClockInfo()
{
    time_t now = 0;
    struct tm* ts = { 0 };
    struct sysinfo s_info = { 0 };
    time_t uptime_seconds = 0;

    char current_datetime[256];
    char boot_datetime[256];

    now = time(NULL);

    ts = localtime(&now);
    if (ts == NULL) {
        throw std::runtime_error("Failed to get current datetime.");
    }

    strftime(current_datetime, sizeof(current_datetime), "%FT%TZ", ts);

    if (sysinfo(&s_info) != 0) {
        throw std::runtime_error("Failed to get system uptime.");
    }

    uptime_seconds = s_info.uptime;

    time_t diff = now - uptime_seconds;

    ts = localtime(&diff);
    if (ts == NULL) {
        throw std::runtime_error("Failed to get boot datetime.");
    }

    strftime(boot_datetime, sizeof(boot_datetime), "%FT%TZ", ts);

    return {
        .BootDatetime = boot_datetime,
        .CurrentDatetime = current_datetime,
    };
}
}