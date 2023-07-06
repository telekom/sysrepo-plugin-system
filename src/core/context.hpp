#pragma once

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/Connection.hpp>
#include <optional>

namespace ietf::sys {
/**
 * @brief Plugin context.
 */
class PluginContext {
public:
    /**
     * sysrepo-plugin-generator: Generated constructor for plugin context.
     *
     * @param sess Plugin session from the plugin init callback.
     *
     */
    PluginContext(sysrepo::Session sess);

    /**
     * sysrepo-plugin-generator: Generated getter for the plugin session.
     *
     * @return Plugin session from the init callback.
     *
     */
    sysrepo::Session& getSession();

    /**
     * Get the sysrepo connection.
     *
     * @return Connection to sysrepo.
     *
     */
    sysrepo::Connection getConnection();

    /**
     * sysrepo-plugin-generator: Generated getter for the subscription handle.
     *
     * @return Subscription handle.
     *
     */
    std::optional<sysrepo::Subscription>& getSubscriptionHandle();

    /**
     * sysrepo-plugin-generator: Generated default destructor for plugin context.
     */
    ~PluginContext() = default;

private:
    sysrepo::Session m_sess; ///< Plugin session from the plugin init callback.

    // Subcription handle
    std::optional<sysrepo::Subscription> m_subHandle;
};
}
