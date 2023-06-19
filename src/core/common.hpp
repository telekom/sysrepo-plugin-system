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

}
