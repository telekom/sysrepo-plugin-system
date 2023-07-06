#include "context.hpp"
#include "sysrepo-cpp/Session.hpp"

namespace ietf::sys {
/**
 * sysrepo-plugin-generator: Generated constructor for plugin context.
 *
 * @param sess Plugin session from the plugin init callback.
 *
 */
PluginContext::PluginContext(sysrepo::Session sess)
    : m_sess(sess)
{
}

/**
 * sysrepo-plugin-generator: Generated getter for the plugin session.
 *
 * @return Plugin session from the init callback.
 *
 */
sysrepo::Session& PluginContext::getSession() { return m_sess; }

/**
 * Get the sysrepo connection.
 *
 * @return Connection to sysrepo.
 *
 */
sysrepo::Connection PluginContext::getConnection() { return getSession().getConnection(); }

/**
 * sysrepo-plugin-generator: Generated getter for the subscription handle.
 *
 * @return Subscription handle.
 *
 */
std::optional<sysrepo::Subscription>& PluginContext::getSubscriptionHandle() { return m_subHandle; }

}
