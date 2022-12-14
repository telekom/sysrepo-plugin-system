#pragma once

#include "Context.hpp"
#include "srpcpp/Plugin.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include <sysrepo.h>

extern "C" {
int sr_plugin_init_cb(sr_session_ctx_t *session, void **priv);
void sr_plugin_cleanup_cb(sr_session_ctx_t *session, void *priv);
}

class SystemPlugin : public srpc::Plugin<ietf::sys::Context>
{
  public:
	SystemPlugin(ietf::sys::Context *ctx)
		: srpc::Plugin<ietf::sys::Context>(ctx)
	{
	}

	sysrepo::ErrorCode initData() override
	{
		sysrepo::ErrorCode error = sysrepo::ErrorCode::Ok;

		// initializes context data and checks the startup datastore to see if any changes are applied on the system

		return error;
	}

	sysrepo::ErrorCode setupSubscriptions() override
	{
		sysrepo::ErrorCode error = sysrepo::ErrorCode::Ok;
		return error;
	}

	~SystemPlugin()
	{
	}
};