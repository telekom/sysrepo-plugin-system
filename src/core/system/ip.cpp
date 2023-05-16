#include "ip.hpp"

//////////////////////////////////////////////
//              IPV4Address                //
/////////////////////////////////////////////
namespace ietf::sys::ipv {

IPV4Address::IPV4Address(std::string ipv4_address)
    : Address()
{
    this->address = ipv4_address;
    struct in_addr ip_addr;
    if (inet_pton(AF_INET, address.c_str(), &ip_addr) == 1) {
        // its ipv4
        BYTE_SIZE = 4;
        version = 2;

        uint8_t* bytes = (uint8_t*)&ip_addr.s_addr;
        for (int i = 0; i < BYTE_SIZE; i++) {
            byte_vector.push_back(bytes[i]);
        };
    } else {
        throw std::runtime_error("Wrong IPV4 Format!");
    }
};

IPV4Address::IPV4Address(std::vector<uint8_t> bytes)
{
    if (bytes.size() == 4) {
        BYTE_SIZE = 4;
        version = 2;

        for (int i = 0; i < BYTE_SIZE; i++) {

            byte_vector.push_back(bytes.at(i));
            address.append(std::to_string(bytes[i]));

            if (i < (BYTE_SIZE - 1)) {
                address.append(".");
            };
        }
    } else {
        throw std::runtime_error("Wrong IPV4 Size Format!");
    }
}

//////////////////////////////////////////////
//              IPV6Address                //
/////////////////////////////////////////////

IPV6Address::IPV6Address(std::string ipv6_address)
    : Address()
{

    this->address = ipv6_address;
    struct in6_addr ip_addr;
    if (inet_pton(AF_INET6, address.c_str(), &ip_addr) == 1) {
        // its ipv6
        BYTE_SIZE = 16;
        version = 10;

        uint8_t* bytes = (uint8_t*)&ip_addr;
        for (int i = 0; i < BYTE_SIZE; i++) {
            byte_vector.push_back(bytes[i]);
        };
    } else {
        throw std::runtime_error("Wrong IPV6 Format!");
    }
};

IPV6Address::IPV6Address(std::vector<uint8_t> bytes)
{
    if (bytes.size() == 16) {
        BYTE_SIZE = 16;
        version = 10;

        for (int i = 0; i < BYTE_SIZE; i++) {

            byte_vector.push_back(bytes.at(i));
            address.append(std::to_string(bytes[i]));

            if (i < (BYTE_SIZE - 1) && i%2 == 1) {
                address.append(":");
            };
        }
    } else {
        throw std::runtime_error("Wrong IPV6 Size Format!");
    }
}

int Address::getVersion() { return version; };

std::string Address::getStringAddr() { return this->address; };

std::vector<uint8_t> Address::byteVector() { return byte_vector; };

bool Address::operator==(const Address& other) const
{

    // first check the version
    if (this->version != other.version) {
        return false;
    }

    // check byte size
    if (this->BYTE_SIZE != other.BYTE_SIZE) {
        return false;
    }

    // check byte array
    for (int i = 0; i < BYTE_SIZE; i++) {
        if (byte_vector.at(i) != other.byte_vector.at(i)) {
            return false;
        }
    }

    // finaly if all ok, return true, they are equal
    return true;
}
}
// End of address implementation
/////////////////////////////////////////////////////////////