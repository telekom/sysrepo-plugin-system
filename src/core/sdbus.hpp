#pragma once

#include <sdbus-c++/sdbus-c++.h>
#include <string>

namespace ietf::sys {

// [TODO]: Document this class
template <class GET, class... SET> class SdBus {
public:
    SdBus(std::string destination, std::string objectPath, std::string interface, std::string set_method, std::string get_method)
        : m_dest(destination)
        , m_objPath(objectPath)
        , m_interface(interface)
        , m_setMethod(set_method)
        , m_getMethod(get_method)
    {
    }

    bool exportToSdBus(SET... data)
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
    }

    GET importFromSdBus()
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

private:
    std::string m_dest;
    std::string m_objPath;
    std::string m_interface;
    std::string m_setMethod;
    std::string m_getMethod;
};
}
