#include "plugin.hpp"
#include "core/common.hpp"
#include "core/context.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

#include <srpcpp.hpp>

#ifdef SYSTEM_MODULE
#include "modules/system.hpp"
#endif

#ifdef HOSTNAME_MODULE
#include "modules/hostname.hpp"
#endif

#ifdef TIMEZONE_MODULE
#include "modules/timezone-name.hpp"
#endif

#ifdef DNS_MODULE
#include "modules/dns.hpp"
#endif

#ifdef NTP_MODULE
#include "modules/ntp.hpp"
#endif

#ifdef AUTH_MODULE
#include "modules/auth.hpp"
#endif

#include "sysrepo.h"

namespace sr = sysrepo;

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
    auto& registry(srpc::ModuleRegistry<ietf::sys::PluginContext>::getInstance());
    auto ctx = new ietf::sys::PluginContext(sess);

    *priv = static_cast<void*>(ctx);

    // create session subscriptions
    SRPLG_LOG_INF(ctx->getPluginName(), "Creating plugin subscriptions");

#ifdef SYSTEM_MODULE
    registry.registerModule<SystemModule>(*ctx);
#endif

#ifdef HOSTNAME_MODULE
    registry.registerModule<HostnameModule>(*ctx);
#endif

#ifdef TIMEZONE_MODULE
    registry.registerModule<TimezoneModule>(*ctx);
#endif

#ifdef DNS_MODULE
    registry.registerModule<DnsModule>(*ctx);
#endif

#ifdef NTP_MODULE
    registry.registerModule<NtpModule>(*ctx);
#endif

#ifdef AUTH_MODULE
    registry.registerModule<AuthModule>(*ctx);
#endif

    // get all registered modules
    auto& modules = registry.getRegisteredModules();

    // for all registered modules - apply startup datastore values
    // startup datastore values are coppied into the running datastore when the first connection with sysrepo is made
    sess.switchDatastore(sr::Datastore::Startup);
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Applying startup values for module %s", mod->getName());
        for (auto& applier : mod->getValueAppliers()) {
            try {
                applier->applyDatastoreValues(sess);
            } catch (const std::runtime_error& err) {
                SRPLG_LOG_ERR(ctx->getPluginName(), "Failed to apply datastore values for the following paths:");
                for (const auto& path : applier->getPaths()) {
                    SRPLG_LOG_ERR(ctx->getPluginName(), "\t%s", path.c_str());
                }
                SRPLG_LOG_ERR(ctx->getPluginName(), "Reason: %s", err.what());
            }
        }
    }

    // get registered modules and create subscriptions
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering operational callbacks for module %s", mod->getName());
        srpc::registerOperationalSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering module change callbacks for module %s", mod->getName());
        srpc::registerModuleChangeSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registering RPC callbacks for module %s", mod->getName());
        srpc::registerRpcSubscriptions(sess, *ctx, mod);
        SRPLG_LOG_INF(ctx->getPluginName(), "Registered module %s", mod->getName());
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
    auto& registry(srpc::ModuleRegistry<ietf::sys::PluginContext>::getInstance());
    auto ctx = static_cast<ietf::sys::PluginContext*>(priv);
    const auto plugin_name = ctx->getPluginName();

    SRPLG_LOG_INF(plugin_name, "Plugin cleanup called");

    auto& modules = registry.getRegisteredModules();
    for (auto& mod : modules) {
        SRPLG_LOG_INF(ctx->getPluginName(), "Cleaning up module: %s", mod->getName());
    }

    // cleanup context manually
    delete ctx;

    SRPLG_LOG_INF(plugin_name, "Plugin cleanup finished");
}
