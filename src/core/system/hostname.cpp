#include "hostname.hpp"

#include <core/common.hpp>

// sethostname() and gethostname()
#include <unistd.h>
#include <stdexcept>

namespace ietf::sys {
/**
 * @brief Get hostname.
 *
 * @return Hostname.
 */
Hostname getHostname()
{
    char hostname[ietf::sys::HOSTNAME_MAX_LEN + 1] = { 0 };

    if (gethostname(hostname, sizeof(hostname)) < 0) {
        throw std::runtime_error("Failed to get hostname.");
    }

    return hostname;
}

/**
 * @brief Set system hostname. Throws a runtime_error if unable to set hostname.
 *
 * @param hostname Hostname.
 */
void setHostname(const Hostname& hostname)
{
    if (auto err = sethostname(hostname.c_str(), hostname.size()); err != 0) {
        throw std::runtime_error("Failed to set hostname.");
    }
}
}