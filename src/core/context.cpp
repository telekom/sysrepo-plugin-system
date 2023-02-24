#include "context.hpp"

namespace ietf::sys {
/**
 * Add a subscription to the list of subscriptions.
 *
 * @param sub Subscription to add.
 *
 */
void OperCtx::addSubscription(sysrepo::Session& sess, const std::string& xpath, sysrepo::OperGetCb callback)
{
    if (m_sub) {
        m_sub->onOperGet("ietf-system", callback, xpath);
    } else {
        m_sub = sess.onOperGet("ietf-system", callback, xpath);
    }
}

/**
 * sysrepo-plugin-generator: Generated constructor for plugin context.
 *
 * @param sess Plugin session from the plugin init callback.
 *
 */
PluginCtx::PluginCtx(sysrepo::Session sess)
    : m_sess(sess)
{
    m_operCtx = std::make_shared<OperCtx>();
    m_moduleChangeCtx = std::make_shared<ModuleChangeCtx>();
    m_rpcCtx = std::make_shared<RPCCtx>();
    m_notifCtx = std::make_shared<NotifCtx>();
}

/**
 * sysrepo-plugin-generator: Generated getter for the plugin session.
 *
 * @return Plugin session from the init callback.
 *
 */
sysrepo::Session& PluginCtx::getSession() { return m_sess; }

/**
 * sysrepo-plugin-generator: Generated getter for the operational data context.
 *
 * @return Operational data context.
 *
 */
std::shared_ptr<OperCtx>& PluginCtx::getOperCtx() { return m_operCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the module change context.
 *
 * @return Module change context.
 *
 */
std::shared_ptr<ModuleChangeCtx>& PluginCtx::getModuleChangeCtx() { return m_moduleChangeCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the RPC context.
 *
 * @return RPC context.
 *
 */
std::shared_ptr<RPCCtx>& PluginCtx::getRPCCtx() { return m_rpcCtx; }

/**
 * sysrepo-plugin-generator: Generated getter for the notification context.
 *
 * @return Notification context.
 *
 */
std::shared_ptr<NotifCtx>& PluginCtx::getNotifCtx() { return m_notifCtx; }

}