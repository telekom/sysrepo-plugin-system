#include "ip.hpp"
#include <arpa/inet.h>
#include <sys/socket.h>

namespace ietf::sys::ip {
/**
 * @brief Default constructor.
 */
Ipv4Address::Ipv4Address(const std::string& address)
{
    struct in_addr ip_addr;

    // convert the string value to a vector of bytes using standard C API
    if (inet_pton(AF_INET, address.c_str(), &ip_addr) == 1) {
        std::byte* bytes = (std::byte*)&ip_addr.s_addr;
        constexpr auto bytes_count = 4;
        for (int i = 0; i < bytes_count; i++) {
            m_bytes.push_back(bytes[i]);
        }
    } else {
        throw std::runtime_error("Invalid IPv4 address");
    }
}

/**
 * @brief Return the version of the IP address (AF_INET or AF_INET6).
 */
int Ipv4Address::getVersion() { return AF_INET; }

/**
 * @brief Return the IP address as bytes vector.
 */
std::vector<std::byte> Ipv4Address::asBytes() { return m_bytes; }

/**
 * @brief Return the IP address as a string.
 */
std::string Ipv4Address::asString()
{
    // convert the bytes vector back to a string value
    constexpr auto IPV4_BUFFER_SIZE = 3 * 4 + 3 + 1;
    char ip_buffer[IPV4_BUFFER_SIZE] = { 0 };

    if (inet_ntop(AF_INET, m_bytes.data(), ip_buffer, IPV4_BUFFER_SIZE) == nullptr) {
        throw std::runtime_error("Unable to convert IPv4 address bytes to a string");
    }

    return std::string(ip_buffer);
}

/**
 * @brief Default constructor.
 */
Ipv6Address::Ipv6Address(const std::string& address)
{
    struct in_addr ip_addr;

    // convert the string value to a vector of bytes using standard C API
    if (inet_pton(AF_INET6, address.c_str(), &ip_addr) == 1) {
        std::byte* bytes = (std::byte*)&ip_addr.s_addr;
        constexpr auto bytes_count = 16;
        for (int i = 0; i < bytes_count; i++) {
            m_bytes.push_back(bytes[i]);
        }
    } else {
        throw std::runtime_error("Invalid IPv6 address");
    }
}

/**
 * @brief Return the version of the IP address (AF_INET or AF_INET6).
 */
int Ipv6Address::getVersion() { return AF_INET6; }

/**
 * @brief Return the IP address as bytes vector.
 */
std::vector<std::byte> Ipv6Address::asBytes() { return m_bytes; }

/**
 * @brief Return the IP address as a string.
 */
std::string Ipv6Address::asString()
{
    // convert the bytes vector back to a string value
    constexpr auto IPV6_BUFFER_SIZE = 8 * 4 + 7;
    char ip_buffer[IPV6_BUFFER_SIZE] = { 0 };

    if (inet_ntop(AF_INET6, m_bytes.data(), ip_buffer, IPV6_BUFFER_SIZE) == nullptr) {
        throw std::runtime_error("Unable to convert IPv6 address bytes to a string");
    }

    return std::string(ip_buffer);
}
}
