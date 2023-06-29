#pragma once

#include <optional>
#include <string>

#include <libyang-cpp/DataNode.hpp>

namespace ly = libyang;

namespace ietf::sys {
namespace yang::tree {
    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createSystemStateContainer();

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/clock
     *
     * @param system_state Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createSystemStateClockContainer(ly::DataNode& system_state);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/clock/boot-datetime
     *
     * @param clock Parent of the node being created.
     * @param boot_datetime Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createClockBootDatetimeLeaf(ly::DataNode& clock, const std::string& boot_datetime);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/clock/current-datetime
     *
     * @param clock Parent of the node being created.
     * @param current_datetime Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createClockCurrentDatetimeLeaf(ly::DataNode& clock, const std::string& current_datetime);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/platform
     *
     * @param system_state Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createSystemStatePlatformContainer(ly::DataNode& system_state);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/platform/machine
     *
     * @param platform Parent of the node being created.
     * @param machine Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createPlatformMachineLeaf(ly::DataNode& platform, const std::string& machine);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/platform/os-version
     *
     * @param platform Parent of the node being created.
     * @param os_version Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createPlatformOsVersionLeaf(ly::DataNode& platform, const std::string& os_version);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/platform/os-release
     *
     * @param platform Parent of the node being created.
     * @param os_release Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createPlatformOsReleaseLeaf(ly::DataNode& platform, const std::string& os_release);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system-state/platform/os-name
     *
     * @param platform Parent of the node being created.
     * @param os_name Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createPlatformOsNameLeaf(ly::DataNode& platform, const std::string& os_name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createSystemContainer();

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/authentication
     *
     * @param system Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createAuthenticationContainer(ly::DataNode& system);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/authentication/user[name='%s']
     *
     * @param authentication Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createAuthUserList(ly::DataNode& authentication, const std::string& name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/authentication/user-authentication-order
     *
     * @param authentication Parent of the node being created.
     * @param user_authentication_order Value of the leaf-list element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createAuthUserAuthenticationOrderLeafList(ly::DataNode& authentication, const std::string& user_authentication_order);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/radius
     *
     * @param system Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createRadiusContainer(ly::DataNode& system);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/radius/options
     *
     * @param radius Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createRadiusOptionsContainer(ly::DataNode& radius);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/radius/options/attempts
     *
     * @param options Parent of the node being created.
     * @param attempts Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createRadiusAttemptsLeaf(ly::DataNode& options, const std::string& attempts);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/radius/options/timeout
     *
     * @param options Parent of the node being created.
     * @param timeout Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createRadiusTimeoutLeaf(ly::DataNode& options, const std::string& timeout);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/radius/server[name='%s']
     *
     * @param radius Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createRadiusServerList(ly::DataNode& radius, const std::string& name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver
     *
     * @param system Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsResolverContainer(ly::DataNode& system);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver/options
     *
     * @param dns_resolver Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsOptionsContainer(ly::DataNode& dns_resolver);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver/options/attempts
     *
     * @param options Parent of the node being created.
     * @param attempts Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsAttemptsLeaf(ly::DataNode& options, const std::string& attempts);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver/options/timeout
     *
     * @param options Parent of the node being created.
     * @param timeout Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsTimeoutLeaf(ly::DataNode& options, const std::string& timeout);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver/server[name='%s']
     *
     * @param dns_resolver Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsServerList(ly::DataNode& dns_resolver, const std::string& name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/dns-resolver/search
     *
     * @param dns_resolver Parent of the node being created.
     * @param search Value of the leaf-list element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createDnsSearchLeafList(ly::DataNode& dns_resolver, const std::string& search);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/ntp
     *
     * @param system Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createNtpContainer(ly::DataNode& system);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/ntp/server[name='%s']
     *
     * @param ntp Parent of the node being created.
     * @param name Key value for key name.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createNtpServerList(ly::DataNode& ntp, const std::string& name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/ntp/enabled
     *
     * @param ntp Parent of the node being created.
     * @param enabled Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createNtpEnabledLeaf(ly::DataNode& ntp, const std::string& enabled);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/clock
     *
     * @param system Parent of the node being created.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createClockContainer(ly::DataNode& system);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/clock/timezone-utc-offset
     *
     * @param clock Parent of the node being created.
     * @param timezone_utc_offset Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createClockTimezoneUtcOffsetLeaf(ly::DataNode& clock, const std::string& timezone_utc_offset);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/clock/timezone-name
     *
     * @param clock Parent of the node being created.
     * @param timezone_name Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createClockTimezoneNameLeaf(ly::DataNode& clock, const std::string& timezone_name);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/location
     *
     * @param system Parent of the node being created.
     * @param location Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createLocationLeaf(ly::DataNode& system, const std::string& location);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/hostname
     *
     * @param system Parent of the node being created.
     * @param hostname Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createHostnameLeaf(ly::DataNode& system, const std::string& hostname);

    /**
     * sysrepo-plugin-generator: Generated yang tree function for path /ietf-system:system/contact
     *
     * @param system Parent of the node being created.
     * @param contact Value of the leaf element.
     *
     * @return Returns the first created node. If no nodes were created, returns std::nullopt.
     *
     */
    std::optional<ly::DataNode> createContactLeaf(ly::DataNode& system, const std::string& contact);

}
}
