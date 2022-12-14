#pragma once

#include "data/DNS.hpp"
#include "srpcpp/Context.hpp"
#include "sysrepo-cpp/Enum.hpp"
#include "sysrepo-cpp/Subscription.hpp"
#include <srpcpp.hpp>

namespace ietf::sys
{
class Context : public srpc::Context
{
  public:
	Context(sr_session_ctx_t *session)
		: srpc::Context(session), m_startupSession(m_session.getConnection().sessionStart(sysrepo::Datastore::Startup))
	{
	}

  private:
	// context data
	sysrepo::Session m_startupSession;

	std::list<dns::Server> m_dnsServers;
};
} // namespace ietf::sys