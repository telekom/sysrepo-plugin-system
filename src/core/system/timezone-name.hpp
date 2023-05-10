#pragma once

#include <core/sdbus.hpp>

namespace ietf::sys {

// it sends a string and a boolean, receives string
class Timezone : public SdBUS<std::string, std::string, bool> {
public:
    Timezone();

    bool setTimezone(std::string timezone);
    std::string getTimezone();
};

}