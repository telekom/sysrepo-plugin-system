#pragma once

#include <string>

namespace ietf::sys {

/**
 * @brief Hostname type alias.
 */
using TimezoneName = std::string;

/**
 * @brief Get system timezone name from /etc/localtime.
 *
 * @return Timezone name.
 */
TimezoneName getTimezoneName();

/**
 * @brief Set system timezone name. Throws a runtime_error if unable to set timezone.
 *
 * @param timezoneName Timezone name.
 */
void setTimezoneName(const TimezoneName& timezone_name);

}