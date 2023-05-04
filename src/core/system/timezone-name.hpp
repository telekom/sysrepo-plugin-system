#pragma once

#include <string>
#include <sdbus-c++/sdbus-c++.h>

namespace ietf::sys {

// set and get types
template <class GET, class ...SET> class SdBUS {
public:
    SdBUS(std::string destination, std::string objectPath, std::string interface, std::string setmethodname, std::string getmethodname);

protected:
    bool exportToSdBus(SET ...data);
    GET importFromSdBus();

private:
    std::string dest, objPath, interface, sdbusSetMethod, sdbusGetMethod;
};

// it sends a string and a boolean, receives string
class Timezone : public SdBUS<std::string , std::string, bool> {
public:
    Timezone();

    bool setTimezone(std::string timezone);
    std::string getTimezone();
};

}