#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>

namespace ietf::sys::ip {

// [TODO]: Document class
class Address {
public:
    virtual int getVersion();
    virtual std::string getStringAddr();

    virtual std::vector<uint8_t> byteVector();

    virtual bool operator==(const Address& other) const;

protected:
    uint8_t BYTE_SIZE;
    std::string address;
    int version;
    std::vector<uint8_t> byte_vector;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class Ipv4Address : public Address {
public:
    Ipv4Address() = delete;
    Ipv4Address(std::string address);
    Ipv4Address(std::vector<uint8_t> bytes);
};

class Ipv6Address : public Address {
public:
    Ipv6Address() = delete;
    Ipv6Address(std::string address);
    Ipv6Address(std::vector<uint8_t> bytes);
};
}
////////////////////////////////////////////////////////////////////////////////////////////////////
