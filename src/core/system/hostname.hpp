#pragma once

#include <string>

namespace ietf::sys {
/**
 * @brief Hostname type alias.
 */
using Hostname = std::string;

/**
 * @brief Get hostname.
 *
 * @return Hostname.
 */
Hostname getHostname();

/**
 * @brief Set system hostname. Throws a runtime_error if unable to set hostname.
 *
 * @param hostname Hostname.
 */
void setHostname(const Hostname& hostname);
}