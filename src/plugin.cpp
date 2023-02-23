#include "plugin.hpp"

#include <sysrepo-cpp/Session.hpp>

namespace sr = sysrepo;

int sr_plugin_init_cb(sr_session_ctx_t* session, void** priv)
{
    sr::ErrorCode error = sysrepo::ErrorCode::Ok;
    return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t* session, void* priv) { }