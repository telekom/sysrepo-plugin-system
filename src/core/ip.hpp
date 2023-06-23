#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <arpa/inet.h>
#include <stdexcept>

namespace ietf::sys::ip {
/**
 * @brief IP address interface.
 */
class IAddress {
public:
    /**
     * @brief Return the version of the IP address (AF_INET or AF_INET6).
     */
    virtual int getVersion() = 0;

    /**
     * @brief Return the IP address as bytes vector.
     */
    virtual std::vector<uint8_t> asBytes() = 0;

    /**
     * @brief Return the IP address as a string.
     */
    virtual std::string asString() = 0;

    virtual ~IAddress() { }

protected:
    std::vector<uint8_t> m_bytes; ///< Bytes of the IP address.
};

/**
 * @brief IPv4 address class. Represents a single IPv4 address.
 */
class Ipv4Address : public IAddress {
public:
    /**
     * @brief Default constructor.
     */
    Ipv4Address(const std::string& address);

    /**
     * @brief Default constructor.
     */
    Ipv4Address(std::vector<uint8_t>& bytes);

    /**
     * @brief Return the version of the IP address (AF_INET or AF_INET6).
     */
    virtual int getVersion() override;

    /**
     * @brief Return the IP address as bytes vector.
     */
    virtual std::vector<uint8_t> asBytes() override;

    /**
     * @brief Return the IP address as a string.
     */
    virtual std::string asString() override;
};

/**
 * @brief IPv6 address class. Represents a single IPv6 address.
 */
class Ipv6Address : public IAddress {
public:
    /**
     * @brief Default constructor.
     */
    Ipv6Address(const std::string& address);

    /**
     * @brief Default constructor.
     */
    Ipv6Address(std::vector<uint8_t>& bytes);

    /**
     * @brief Return the version of the IP address (AF_INET or AF_INET6).
     */
    int getVersion() override;

    /**
     * @brief Return the IP address as bytes vector.
     */
    std::vector<uint8_t> asBytes() override;

    /**
     * @brief Return the IP address as a string.
     */
    std::string asString() override;
};
}
