#include "context.hpp"
#include "srpcpp/context.hpp"
#include "sysrepo-cpp/Session.hpp"

namespace ietf::sys {
/**
 * @brief Default constructor.
 *
 * @param sess Plugin session from the plugin init callback.
 *
 */
PluginContext::PluginContext(sysrepo::Session sess)
    : srpc::BasePluginContext(sess)
{
}

/**
 * @brief Default destructor.
 */
PluginContext::~PluginContext() { }
}
