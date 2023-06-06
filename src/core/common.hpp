#pragma once

#include "libyang-cpp/DataNode.hpp"
#include <string>
#include <map>

#include <libyang-cpp/Context.hpp>

namespace ly = libyang;

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

/**
 * @brief Get meta value.
 *
 * @param meta Meta collection object.
 * @param name Meta name.
 * @return std::string Meta value.
 */
const std::string getMetaValue(ly::MetaCollection& meta, const std::string& name);

/**
 * @brief Convert meta values list to a hash.
 *
 * @param meta Meta collection object.
 * @return std::map<std::string, std::string> Meta values hash.
 */
std::map<std::string, std::string> getMetaValuesHash(ly::MetaCollection meta);

}
