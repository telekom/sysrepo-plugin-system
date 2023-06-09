#pragma once

#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>

namespace ietf::sys::ipv {

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

class IPV4Address : public Address {
public:
    IPV4Address() = delete;
    IPV4Address(std::string address);
    IPV4Address(std::vector<uint8_t> bytes);
};

class IPV6Address : public Address {
public:
    IPV6Address() = delete;
    IPV6Address(std::string address);
    IPV6Address(std::vector<uint8_t> bytes);
};
}
////////////////////////////////////////////////////////////////////////////////////////////////////
