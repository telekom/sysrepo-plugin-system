#pragma once

#include <sysrepo-cpp/Session.hpp>
#include <sysrepo-cpp/Connection.hpp>
#include <optional>
#include <srpcpp/context.hpp>

namespace ietf::sys {
/**
 * @brief Plugin context.
 */
class PluginContext : public srpc::BasePluginContext {
public:
    /**
     * Default constructor.
     *
     * @param sess Plugin session from the plugin init callback.
     *
     */
    PluginContext(sysrepo::Session sess);

    /**
     * @brief Get the name of the plugin which uses this context.
     *
     * @return Plugin name string.
     */
    virtual constexpr const char* getPluginName() override { return "ietf-system-plugin"; }

    /**
     * @brief Default destructor.
     */
    ~PluginContext();
};
}
