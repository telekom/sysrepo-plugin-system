#include "plugin.hpp"
#include "core/context.hpp"

// subscription API
#include "core/sub/change.hpp"
#include "core/sub/oper.hpp"
#include "core/sub/rpc.hpp"

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/utils/utils.hpp>

namespace sr = sysrepo;

int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    auto sess = sysrepo::wrapUnmanagedSession(session);
    auto plugin_ctx = new ietf::sys::PluginCtx(sess);

    *priv = static_cast<void*>(plugin_ctx);

    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv)
{
    auto plugin_ctx = static_cast<ietf::sys::PluginCtx*>(priv);
    delete plugin_ctx;
}