#pragma once

#include <string>
#include <core/sdbus.hpp>

namespace ietf::sys {

class Hostname : public SdBUS<std::string, std::string, bool> {
public:
    Hostname();
    bool setHostname(std::string);
    std::string getHostname();
};

}