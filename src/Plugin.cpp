#include "Plugin.hpp"
#include "Context.hpp"
#include "srpcpp/Common.hpp"
#include "srpcpp/Context.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include "sysrepo-cpp/utils/utils.hpp"

#include <memory>
#include <sysrepo.h>

extern "C" {
#include <systemd/sd-bus.h>
}

#include <srpcpp.hpp>

namespace sr = sysrepo;

int sr_plugin_init_cb(sr_session_ctx_t *session, void **priv)
{
	sr::ErrorCode error = sysrepo::ErrorCode::Ok;

	auto sys_plugin = new SystemPlugin(new ietf::sys::Context(session));

	// init plugin data
	error = sys_plugin->initData();
	if (error != sysrepo::ErrorCode::Ok) {
		// error
	}

	error = sys_plugin->setupSubscriptions();
	if (error != sysrepo::ErrorCode::Ok) {
	}

	// setup private data
	*priv = static_cast<void *>(sys_plugin);

	// plugin is started

	return static_cast<int>(error);
}

void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *priv)
{
	SystemPlugin *plugin = static_cast<SystemPlugin *>(priv);

	SRPLG_LOG_INF("system-plugin", "Ending Plugin");

	delete plugin;
}