#include "hostname.hpp"


namespace ietf::sys {

    Hostname::Hostname() : SdBUS<std::string, std::string, bool>(
        "org.freedesktop.hostname1",
        "/org/freedesktop/hostname1",
        "org.freedesktop.hostname1",
        "SetStaticHostname",
        "Hostname"
    ){}

    std::string Hostname::getHostname(){
        return importFromSdBus();
    }

    bool Hostname::setHostname(std::string hostname){
        // boolean parameter is " Interactive ", maybe default true?
        // exportToSdBus returns true if error
        return (!exportToSdBus(hostname,true));
    }

}
