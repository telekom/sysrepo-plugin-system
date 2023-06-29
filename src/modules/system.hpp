#pragma once

#include <srpcpp/module.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include <sysrepo.h>

// helpers
namespace sr = sysrepo;
namespace ly = libyang;

/**
 * Operational context for the system module.
 */
class SystemOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the system module.
 */
class SystemModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the system module.
 */
class SystemRpcContext : public srpc::IModuleContext { };

namespace ietf::sys::state {
/**
 * @brief Platform information helper struct.
 */
struct PlatformInfo {
    std::string OsName; ///< OS name.
    std::string OsRelease; ///< OS release.
    std::string OsVersion; ///< OS version.
    std::string Machine; ///< Machine.
};

/**
 * @brief Get platform information.
 *
 * @return Platform information.
 */
PlatformInfo getPlatformInfo();

/**
 * @brief Clock information helper struct.
 */
struct ClockInfo {
    std::string BootDatetime; ///< Boot datetime.
    std::string CurrentDatetime; ///< Current datetime.
};

/**
 * @brief Get clock information.
 *
 * @return Clock information.
 */
ClockInfo getClockInfo();

/**
 * @brief Set system current datetime. Throws a runtime_error if unable to set datetime.
 *
 * @param datetime Datetime.
 */
static void setCurrentDatetime(const std::string& datetime);

/**
 * @brief Restart system.
 */
static void restartSystem();

/**
 * @brief Shutdown system.
 */
static void shutdownSystem();
}

namespace ietf::sys::sub::oper {
/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/platform.
 */
class StatePlatformOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    StatePlatformOperGetCb(std::shared_ptr<SystemOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/platform.
     *
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param moduleName The module name used for subscribing.
     * @param subXPath The optional xpath used at the time of subscription.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
        std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

private:
    std::shared_ptr<SystemOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system-state/clock.
 */
class StateClockOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    StateClockOperGetCb(std::shared_ptr<SystemOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system-state/clock.
     *
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param moduleName The module name used for subscribing.
     * @param subXPath The optional xpath used at the time of subscription.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * @param output A handle to a tree. The callback is supposed to fill this tree with the requested data.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
        std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output);

private:
    std::shared_ptr<SystemOperationalContext> m_ctx;
};
}

namespace ietf::sys::sub::change {

}

namespace ietf::sys::sub::rpc {
/**
 * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:set-current-datetime.
 */
class SetCurrentDatetimeRpcCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SetCurrentDatetimeRpcCb(std::shared_ptr<SystemRpcContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:set-current-datetime.
     *
     * A callback for RPC/action subscriptions.
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param path Path identifying the RPC/action.
     * @param input Data tree specifying the input of the RPC/action.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
     * Points to the operation root node.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
        uint32_t requestId, ly::DataNode output);

private:
    std::shared_ptr<SystemRpcContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:system-restart.
 */
class SystemRestartRpcCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SystemRestartRpcCb(std::shared_ptr<SystemRpcContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-restart.
     *
     * A callback for RPC/action subscriptions.
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param path Path identifying the RPC/action.
     * @param input Data tree specifying the input of the RPC/action.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
     * Points to the operation root node.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
        uint32_t requestId, ly::DataNode output);

private:
    std::shared_ptr<SystemRpcContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:system-shutdown.
 */
class SystemShutdownRpcCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SystemShutdownRpcCb(std::shared_ptr<SystemRpcContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-shutdown.
     *
     * A callback for RPC/action subscriptions.
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param path Path identifying the RPC/action.
     * @param input Data tree specifying the input of the RPC/action.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
     * Points to the operation root node.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
        uint32_t requestId, ly::DataNode output);

private:
    std::shared_ptr<SystemRpcContext> m_ctx;
};
}

/**
 * @brief System module.
 */
class SystemModule : public srpc::IModule {
public:
    /**
     * System module constructor. Allocates each context.
     */
    SystemModule();

    /**
     * Return the operational context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getOperationalContext() override;

    /**
     * Return the module changes context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getModuleChangesContext() override;

    /**
     * Return the RPC context from the module.
     */
    virtual std::shared_ptr<srpc::IModuleContext> getRpcContext() override;

    /**
     * Get all operational callbacks which the module should use.
     */
    virtual std::list<srpc::OperationalCallback> getOperationalCallbacks() override;

    /**
     * Get all module change callbacks which the module should use.
     */
    virtual std::list<srpc::ModuleChangeCallback> getModuleChangeCallbacks() override;

    /**
     * Get all RPC callbacks which the module should use.
     */
    virtual std::list<srpc::RpcCallback> getRpcCallbacks() override;

    /**
     * Get all system value checkers that this module provides.
     */
    virtual std::list<std::shared_ptr<srpc::DatastoreValuesChecker>> getValueCheckers() override;

    /**
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * System module destructor.
     */
    ~SystemModule() { }

private:
    std::shared_ptr<SystemOperationalContext> m_operContext;
    std::shared_ptr<SystemModuleChangesContext> m_changeContext;
    std::shared_ptr<SystemRpcContext> m_rpcContext;
};
