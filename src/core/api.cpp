#include "api.hpp"
#include "core/context.hpp"
#include "core/common.hpp"

// Platform information
#include <stdexcept>
#include <memory>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

// system() API
#include <cstdlib>
#include <stdexcept>
#include <cstring>

// sethostname() and gethostname()
#include <unistd.h>

#include <filesystem>
#include <fstream>

namespace ietf::sys {
namespace API {
    /**
     * @brief Set system current datetime. Throws a runtime_error if unable to set datetime.
     *
     * @param datetime Datetime.
     */
    void RPC::setCurrentDatetime(const std::string& datetime)
    {
        struct tm t = { 0 };
        time_t time_to_set = 0;
        struct timespec stime = { 0 };

        if (strptime(datetime.c_str(), "%FT%TZ", &t) == 0) {
            throw std::runtime_error("Failed to parse datetime");
        }

        time_to_set = mktime(&t);
        if (time_to_set == -1) {
            throw std::runtime_error("Failed to convert datetime to time_t");
        }

        stime.tv_sec = time_to_set;

        if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
            throw std::runtime_error("Failed to set system time");
        }
    }

    /**
     * @brief Restart system.
     */
    void RPC::restartSystem()
    {
        if (auto rc = std::system("reboot"); rc != 0) {
            throw std::runtime_error("Failed to restart system.");
        }
    }

    /**
     * @brief Shutdown system.
     */
    void RPC::shutdownSystem()
    {
        if (auto rc = std::system("poweroff"); rc != 0) {
            throw std::runtime_error("Failed to shutdown system.");
        }
    }
}
}