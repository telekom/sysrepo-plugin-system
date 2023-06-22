#pragma once

#include "libyang-cpp/DataNode.hpp"
#include <string>
#include <map>

#include <libyang-cpp/Context.hpp>

namespace ietf::sys {

constexpr auto PLUGIN_NAME = "ietf-system-plugin";
constexpr auto HOSTNAME_MAX_LEN = 253;
constexpr auto TIMEZONE_FILE_PATH = "/etc/localtime";
constexpr auto TIMEZONE_DIR_PATH = "/usr/share/zoneinfo";

namespace auth {
    constexpr auto DEFAULT_GECOS = "ietf-system user";
    constexpr auto SKEL_DIR = "/etc/skel";
    constexpr auto PASSWD_PATH = "/etc/passwd";
    constexpr auto DEFAULT_SHELL = "/bin/bash";
}

}
