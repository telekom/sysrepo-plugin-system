#include "plugin.hpp"
#include "core/common.hpp"
#include "core/context.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

#include <srpcpp.hpp>

// [TODO]: Try to remove dependency
#include "modules/system.hpp"
#include "modules/hostname.hpp"
#include "modules/timezone-name.hpp"
#include "modules/dns.hpp"
#include "modules/auth.hpp"
#include "sysrepo.h"

namespace sr = sysrepo;

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod);

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod);

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod);

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
    auto& registry(srpc::ModuleRegistry::getInstance());
    auto ctx = new ietf::sys::PluginContext(sess);

    *priv = static_cast<void*>(ctx);

    // create session subscriptions
    SRPLG_LOG_INF("ietf-system-plugin", "Creating plugin subscriptions");

    // [TODO]: Try to remove this dependency and use static variable in each module to register it
    registry.registerModule<SystemModule>();
    registry.registerModule<HostnameModule>();
    registry.registerModule<TimezoneModule>();
    registry.registerModule<DnsModule>();
    registry.registerModule<AuthModule>();

    // get registered modules and create subscriptions
    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Registering operational callbacks for module %s", mod->getName());
        registerOperationalSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Registering module change callbacks for module %s", mod->getName());
        registerModuleChangeSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Registering RPC callbacks for module %s", mod->getName());
        registerRpcSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Registered module %s", mod->getName());
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

    auto& registry(srpc::ModuleRegistry::getInstance());
    auto ctx = static_cast<ietf::sys::PluginContext*>(priv);

    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Cleaning up module: %s", mod->getName());
    }

    // cleanup context manually
    delete ctx;

    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup finished");
}

/**
 * Register all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod)
{
    const auto oper_callbacks = mod->getOperationalCallbacks();

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : oper_callbacks) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Creating operational subscription for xpath %s", cb.XPath.c_str());
        if (sub_handle.has_value()) {
            sub_handle->onOperGet("ietf-system", cb.Callback, cb.XPath);
        } else {
            sub_handle = sess.onOperGet("ietf-system", cb.Callback, cb.XPath);
        }
    }
}

/**
 * Register all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod)
{
    const auto change_callbacks = mod->getModuleChangeCallbacks();

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : change_callbacks) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Creating module change subscription for xpath %s", cb.XPath.c_str());
        if (sub_handle.has_value()) {
            sub_handle->onModuleChange("ietf-system", cb.Callback, cb.XPath);
        } else {
            sub_handle = sess.onModuleChange("ietf-system", cb.Callback, cb.XPath);
        }
    }
}

/**
 * Register all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 * @param mod Module to use.
 *
 */
void registerRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx, std::unique_ptr<srpc::IModule>& mod)
{
    const auto rpc_callbacks = mod->getRpcCallbacks();

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : rpc_callbacks) {
        SRPLG_LOG_INF(ietf::sys::PLUGIN_NAME, "Creating RPC subscription for xpath %s", cb.XPath.c_str());
        if (sub_handle.has_value()) {
            sub_handle->onRPCAction(cb.XPath, cb.Callback);
        } else {
            sub_handle = sess.onRPCAction(cb.XPath, cb.Callback);
        }
    }
}