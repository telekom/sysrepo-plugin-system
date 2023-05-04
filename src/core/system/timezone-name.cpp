#include "timezone-name.hpp"

#include <core/common.hpp>


namespace ietf::sys {
template <class GET, class ...SET>
SdBUS<GET, SET...>::SdBUS(std::string destination, std::string objectPath, std::string interface, std::string setmethodname, std::string getmethodname)
    : dest { destination }
    , objPath { objectPath }
    , interface {
    interface
}, sdbusSetMethod { setmethodname }, sdbusGetMethod { getmethodname } {};

template <class GET, class ...SET> bool SdBUS<GET, SET...>::exportToSdBus(SET ...data)
{
    bool error = false;

    try {
        auto proxy = sdbus::createProxy(this->dest, this->objPath);
        proxy->callMethod(this->sdbusSetMethod).onInterface(this->interface).withArguments(data...);
    } catch (sdbus::Error& e) {
        throw std::runtime_error(e.getMessage());
        error = true;
    };

    return error;
};

template <class GET, class ...SET> GET SdBUS<GET, SET...>::importFromSdBus()
{
    GET data;

    try {
        auto proxy = sdbus::createProxy(this->dest, this->objPath);
        sdbus::Variant v = proxy->getProperty(this->sdbusGetMethod).onInterface(this->interface);
        data = v.get<GET>();
    } catch (sdbus::Error& e) {
        throw std::runtime_error(e.getMessage());
    }
    return data;
}

Timezone::Timezone()
    : SdBUS<std::string, std::string, bool>(
        "org.freedesktop.timedate1", "/org/freedesktop/timedate1", "org.freedesktop.timedate1", "SetTimezone", "Timezone") {};

bool Timezone::setTimezone(std::string timezone)
{

    return !exportToSdBus(timezone,true);
}

std::string Timezone::getTimezone()
{

    std::string data = importFromSdBus();
    return data;
}
}