#include "plugin.hpp"
#include "core/context.hpp"

// subscription API
#include "core/sub/change.hpp"
#include "core/sub/oper.hpp"
#include "core/sub/rpc.hpp"
#include "sysrepo.h"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

namespace sr = sysrepo;

/**
 * Create all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param oper_ctx Operational context to use for callbacks.
 *
 */
void createOperationalSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::OperCtx> oper_ctx);

/**
 * Create all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param change_ctx Module changes context to use for callbacks.
 *
 */
void createModuleChangeSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::ModuleChangeCtx> change_ctx);

/**
 * Create all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param rpc_ctx RPC context to use for callbacks.
 *
 */
void createRpcSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::RPCCtx> rpc_ctx);

int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    auto sess = sysrepo::wrapUnmanagedSession(session);
    auto plugin_ctx = new ietf::sys::PluginCtx(sess);

    *priv = static_cast<void*>(plugin_ctx);

    // create session subscriptions
    SRPLG_LOG_INF("ietf-system-plugin", "Creating plugin subscriptions");
    createOperationalSubscriptions(sess, plugin_ctx->getOperCtx());
    createModuleChangeSubscriptions(sess, plugin_ctx->getModuleChangeCtx());
    createRpcSubscriptions(sess, plugin_ctx->getRPCCtx());
    SRPLG_LOG_INF("ietf-system-plugin", "Created plugin subscriptions");

    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{
    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup called");

    auto plugin_ctx = static_cast<ietf::sys::PluginCtx*>(priv);
    delete plugin_ctx;

    SRPLG_LOG_INF("ietf-system-plugin", "Plugin cleanup finished");
}

/**
 * Create all operational plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param oper_ctx Operational context to use for callbacks.
 *
 */
void createOperationalSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::OperCtx> oper_ctx)
{
    // platform/os-name
    auto sub = sess.onOperGet("ietf-system", ietf::sys::sub::oper::PlatformOsNameOperGetCb(oper_ctx), "/ietf-system:system-state/platform/os-name");

    // platform/os-release
    sub = sess.onOperGet("ietf-system", ietf::sys::sub::oper::PlatformOsReleaseOperGetCb(oper_ctx), "/ietf-system:system-state/platform/os-release");

    // platform/os-version
    sub = sess.onOperGet("ietf-system", ietf::sys::sub::oper::PlatformOsVersionOperGetCb(oper_ctx), "/ietf-system:system-state/platform/os-version");

    // platform/machine
    sub = sess.onOperGet("ietf-system", ietf::sys::sub::oper::PlatformMachineOperGetCb(oper_ctx), "/ietf-system:system-state/platform/machine");

    // clock/current-datetime
    sub = sess.onOperGet(
        "ietf-system", ietf::sys::sub::oper::ClockCurrentDatetimeOperGetCb(oper_ctx), "/ietf-system:system-state/clock/current-datetime");

    // clock/boot-datetime
    sub = sess.onOperGet("ietf-system", ietf::sys::sub::oper::ClockBootDatetimeOperGetCb(oper_ctx), "/ietf-system:system-state/clock/boot-datetime");
}

/**
 * Create all module change plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param change_ctx Module changes context to use for callbacks.
 *
 */
void createModuleChangeSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::ModuleChangeCtx> change_ctx) { }

/**
 * Create all RPC plugin subscriptions.
 *
 * @param sess Session to use for creating subscriptions.
 * @param rpc_ctx RPC context to use for callbacks.
 *
 */
void createRpcSubscriptions(sr::Session& sess, std::shared_ptr<ietf::sys::RPCCtx> rpc_ctx) { }