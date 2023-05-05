#include "timezone-name.hpp"

namespace ietf::sys {

Timezone::Timezone()
    : SdBUS<std::string, std::string, bool>(
        "org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTimezone", "Timezone") {};

bool Timezone::setTimezone(std::string timezone) { return !exportToSdBus(timezone, true); }

std::string Timezone::getTimezone()
{

    std::string data = importFromSdBus();
    return data;
}

}