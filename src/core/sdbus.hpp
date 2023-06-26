#pragma once

#include <sdbus-c++/sdbus-c++.h>
#include <string>

namespace ietf::sys {

// [TODO]: Document this class
template <class GET, class... SET> class SdBus {
public:
    SdBus(std::string destination, std::string objectPath, std::string interface, std::string set_method, std::string property)
        : m_dest(destination)
        , m_objPath(objectPath)
        , m_interface(interface)
        , m_setMethod(set_method)
        , m_property(property)
    {
    }

protected:
    void exportToSdBus(SET... data)
    {
        try {
            auto proxy = sdbus::createProxy(m_dest, m_objPath);
            proxy->callMethod(m_setMethod).onInterface(m_interface).withArguments(data...);
        } catch (sdbus::Error& e) {
            throw std::runtime_error(e.getMessage());
        };
    }

    GET importFromSdBus()
    {
        GET data;

        try {
            auto proxy = sdbus::createProxy(m_dest, m_objPath);
            sdbus::Variant v = proxy->getProperty(m_property).onInterface(m_interface);
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
    std::string m_property;
};
}
