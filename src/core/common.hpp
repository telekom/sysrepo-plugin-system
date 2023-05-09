#pragma once

#include <string>

namespace ietf::sys {
constexpr auto PLUGIN_NAME = "ietf-system-plugin";
constexpr auto HOSTNAME_MAX_LEN = 253;
constexpr auto TIMEZONE_FILE_PATH = "/etc/localtime";
constexpr auto TIMEZONE_DIR_PATH = "/usr/share/zoneinfo";

/**
 * @brief Extracts the key from the list XPath.
 *
 * @param list List name.
 * @param key Key name.
 * @param xpath XPath of the list.
 * @return std::string Key value.
 */
const std::string extractListKeyFromXPath(const std::string& list, const std::string& key, const std::string& xpath);

}