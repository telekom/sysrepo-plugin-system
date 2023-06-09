#pragma once

#include <sdbus-c++/sdbus-c++.h>
#include <string>

namespace ietf::sys {

// set and get types
template <class GET, class... SET> class SdBUS {
public:
    SdBUS(std::string destination, std::string objectPath, std::string interface, std::string setmethodname, std::string getmethodname);

protected:
    bool exportToSdBus(SET... data);
    GET importFromSdBus();

private:
    std::string dest, objPath, interface, sdbusSetMethod, sdbusGetMethod;
};

template <class GET, class... SET>
SdBUS<GET, SET...>::SdBUS(
    std::string destination, std::string objectPath, std::string interface, std::string setmethodname, std::string getmethodname)
    : dest { destination }
    , objPath { objectPath }
    , interface {
    interface
}, sdbusSetMethod { setmethodname }, sdbusGetMethod { getmethodname } {};

template <class GET, class... SET> bool SdBUS<GET, SET...>::exportToSdBus(SET... data)
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

template <class GET, class... SET> GET SdBUS<GET, SET...>::importFromSdBus()
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

}