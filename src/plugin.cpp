#include "plugin.hpp"
#include "core/common.hpp"
#include "core/context.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

#include "core/callbacks.hpp"
#include "core/module.hpp"
#include "core/module-registry.hpp"

// [TODO]: Try to remove dependency
#include "modules/system.hpp"
#include "modules/hostname.hpp"
#include "modules/timezone-name.hpp"
#include "modules/auth.hpp"

namespace sr = sysrepo;

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

/**
 * @brief Plugin init callback.
 *
 * @param session Plugin session.
 * @param priv Private data.
 *
 * @return Error code (SR_ERR_OK on success).
 */
int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    auto sess = sysrepo::wrapUnmanagedSession(session);
    auto& registry(ModuleRegistry::getInstance());

    // create session subscriptions
    SRPLG_LOG_INF("ietf-system-plugin", "Creating plugin subscriptions");

    // [TODO]: Try to remove this dependency and use static variable in each module to register it
    registry.registerModule<SystemModule>();
    registry.registerModule<HostnameModule>();
    registry.registerModule<TimezoneModule>();
    registry.registerModule<AuthModule>();

    // get registered modules and create subscriptions
    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Registered module: %s", mod->getName());
    }

    SRPLG_LOG_INF("ietf-system-plugin", "Created plugin subscriptions");

    return static_cast<int>(error);
}

/**
 * @brief Plugin cleanup callback.
 *
 * @param session Plugin session.
 * @param priv Private data.
 *
 */
void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{
    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup called");
    auto& registry(ModuleRegistry::getInstance());

    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Cleaning up module: %s", mod->getName());
    }

    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup finished");
}

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx) { }

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx) { }

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void registerRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx) { }
