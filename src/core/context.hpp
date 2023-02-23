#pragma once

#include <sysrepo-cpp/Session.hpp>

namespace ietf::sys {
/**
 * @brief Operational data context.
 */
class OperCtx {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for operational context.
     */
    OperCtx() = default;
};

/**
 * @brief Module changes data context.
 */
class ModuleChangeCtx {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for module change context.
     */
    ModuleChangeCtx() = default;
};

/**
 * @brief RPC data context.
 */
class RPCCtx {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for RPC context.
     */
    RPCCtx() = default;
};

/**
 * @brief Notification data context.
 */
class NotifCtx {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor for notification context.
     */
    NotifCtx() = default;
};

/**
 * @brief Plugin context.
 */
class PluginCtx {
public:
    /**
     * sysrepo-plugin-generator: Generated constructor for plugin context.
     *
     * @param sess Plugin session from the plugin init callback.
     *
     */
    PluginCtx(sysrepo::Session sess);

    /**
     * sysrepo-plugin-generator: Generated getter for the plugin session.
     *
     * @return Plugin session from the init callback.
     *
     */
    sysrepo::Session& getSession();

    /**
     * sysrepo-plugin-generator: Generated getter for the operational data context.
     *
     * @return Operational data context.
     *
     */
    std::shared_ptr<OperCtx>& getOperCtx();

    /**
     * sysrepo-plugin-generator: Generated getter for the module change context.
     *
     * @return Module change context.
     *
     */
    std::shared_ptr<ModuleChangeCtx>& getModuleChangeCtx();

    /**
     * sysrepo-plugin-generator: Generated getter for the RPC context.
     *
     * @return RPC context.
     *
     */
    std::shared_ptr<RPCCtx>& getRPCCtx();

    /**
     * sysrepo-plugin-generator: Generated getter for the notification context.
     *
     * @return Notification context.
     *
     */
    std::shared_ptr<NotifCtx>& getNotifCtx();

    /**
     * sysrepo-plugin-generator: Generated default destructor for plugin context.
     */
    ~PluginCtx() = default;

private:
    sysrepo::Session m_sess; ///< Plugin session from the plugin init callback.

    // Contexts
    std::shared_ptr<OperCtx> m_operCtx; ///< Operational data context.
    std::shared_ptr<ModuleChangeCtx> m_moduleChangeCtx; ///< Module change context.
    std::shared_ptr<RPCCtx> m_rpcCtx; ///< RPC context.
    std::shared_ptr<NotifCtx> m_notifCtx; ///< Notification context.
};
}