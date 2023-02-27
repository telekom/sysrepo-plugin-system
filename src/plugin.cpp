#include "plugin.hpp"
#include "core/context.hpp"

// subscription API
#include "core/sub/change.hpp"
#include "core/sub/oper.hpp"
#include "core/sub/rpc.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include "sysrepo-cpp/Subscription.hpp"
#include "sysrepo.h"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

namespace sr = sysrepo;

struct OperationalCallback {
    std::string xpath;
    sysrepo::OperGetCb callback;
};

struct ModuleChangeCallback {
    std::string xpath;
    sysrepo::ModuleChangeCb callback;
};

struct RPCCallback {
    std::string xpath;
    sysrepo::RpcActionCb callback;
};

/**
 * Create all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

/**
 * Create all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

/**
 * Create all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx);

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
    auto plugin_ctx = new ietf::sys::PluginContext(sess);

    *priv = static_cast<void*>(plugin_ctx);

    // create session subscriptions
    SRPLG_LOG_INF("ietf-system-plugin", "Creating plugin subscriptions");
    createOperationalSubscriptions(sess, *plugin_ctx);
    createModuleChangeSubscriptions(sess, *plugin_ctx);
    createRpcSubscriptions(sess, *plugin_ctx);
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

    auto plugin_ctx = static_cast<ietf::sys::PluginContext*>(priv);
    delete plugin_ctx;

    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup finished");
}

/**
 * Create all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createOperationalSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx)
{
    const auto oper_callbacks = {
        // OperationalCallback { "/ietf-system:system-state/platform/os-name", ietf::sys::sub::oper::PlatformOsNameOperGetCb(ctx.getOperContext()) },
        // OperationalCallback {
        //     "/ietf-system:system-state/platform/os-release", ietf::sys::sub::oper::PlatformOsReleaseOperGetCb(ctx.getOperContext()) },
        // OperationalCallback {
        //     "/ietf-system:system-state/platform/os-version", ietf::sys::sub::oper::PlatformOsVersionOperGetCb(ctx.getOperContext()) },
        // OperationalCallback { "/ietf-system:system-state/platform/machine", ietf::sys::sub::oper::PlatformMachineOperGetCb(ctx.getOperContext()) },
        OperationalCallback { "/ietf-system:system-state/platform", ietf::sys::sub::oper::PlatformOperGetCb(ctx.getOperContext()) },
        OperationalCallback { "/ietf-system:system-state/clock", ietf::sys::sub::oper::ClockOperGetCb(ctx.getOperContext()) },
    };

    auto& sub_handle = ctx.getSubscriptionHandle();

    for (auto& cb : oper_callbacks) {
        if (sub_handle.has_value()) {
            sub_handle->onOperGet("ietf-system", cb.callback, cb.xpath);
        } else {
            sub_handle = sess.onOperGet("ietf-system", cb.callback, cb.xpath);
        }
    }
}

/**
 * Create all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createModuleChangeSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx) { }

/**
 * Create all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param ctx Plugin context.
 *
 */
void createRpcSubscriptions(sr::Session& sess, ietf::sys::PluginContext& ctx) { }