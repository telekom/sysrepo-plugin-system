#pragma once

#include <srpcpp/module.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include "core/context.hpp"
#include "core/sdbus.hpp"
#include "srpcpp/datastore.hpp"

// helpers
namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::sys {
class TimezoneName : public SdBus<std::string, std::string, bool> {
public:
    /**
     * @brief Default constructor.
     */
    TimezoneName();

    /**
     * @brief Get timezone name value from the system.
     *
     * @return System timezone name.
     */
    std::string getValue(void);

    /**
     * @brief Set the timezone name on the system.
     *
     * @param timezone_name Timezone to set.
     */
    void setValue(const std::string& timezone_name);
};
}

/**
 * Operational context for the timezone module.
 */
class TimezoneOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the timezone module.
 */
class TimezoneModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the timezone module.
 */
class TimezoneRpcContext : public srpc::IModuleContext { };

namespace ietf::sys::sub::oper {
/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/clock/timezone-name.
 */
class ClockTimezoneNameOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ClockTimezoneNameOperGetCb(std::shared_ptr<TimezoneOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/clock/timezone-name.
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
    std::shared_ptr<TimezoneOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/clock/timezone-utc-offset.
 */
class ClockTimezoneUtcOffsetOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    ClockTimezoneUtcOffsetOperGetCb(std::shared_ptr<TimezoneOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/clock/timezone-utc-offset.
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
    std::shared_ptr<TimezoneOperationalContext> m_ctx;
};
}

namespace ietf::sys::sub::change {
/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/clock/timezone-name.
 */
class ClockTimezoneNameModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    ClockTimezoneNameModuleChangeCb(std::shared_ptr<TimezoneModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/clock/timezone-name.
     *
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param moduleName The module name used for subscribing.
     * @param subXPath The optional xpath used at the time of subscription.
     * @param event Type of the event that has occured.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * SR_EV_DONE, for example) have the same request ID.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
        sr::Event event, uint32_t requestId);

private:
    std::shared_ptr<TimezoneModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/clock/timezone-utc-offset.
 */
class ClockTimezoneUtcOffsetModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    ClockTimezoneUtcOffsetModuleChangeCb(std::shared_ptr<TimezoneModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/clock/timezone-utc-offset.
     *
     * @param session An implicit session for the callback.
     * @param subscriptionId ID the subscription associated with the callback.
     * @param moduleName The module name used for subscribing.
     * @param subXPath The optional xpath used at the time of subscription.
     * @param event Type of the event that has occured.
     * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
     * SR_EV_DONE, for example) have the same request ID.
     *
     * @return Error code.
     *
     */
    sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName, std::optional<std::string_view> subXPath,
        sr::Event event, uint32_t requestId);

private:
    std::shared_ptr<TimezoneModuleChangesContext> m_ctx;
};
}

/**
 * @brief Checker used to check if ietf-system/system/clock/timezone* value is contained on the system.
 */
class TimezoneValueChecker : public srpc::IDatastoreChecker {
public:
    /**
     * @brief Check for the datastore values on the system.
     *
     * @param session Sysrepo session used for retreiving datastore values.
     *
     * @return Enum describing the output of values comparison.
     */
    virtual srpc::DatastoreValuesCheckStatus checkDatastoreValues(sysrepo::Session& session) override;

    /**
     * @brief Get the paths which the checker is assigned for.
     *
     * @return Checker paths.
     */
    virtual std::list<std::string> getPaths() override
    {
        return {
            "/ietf-system:system/clock/timezone-name",
        };
    }
};

/**
 * @brief Timezone module.
 */
class TimezoneModule : public srpc::IModule<ietf::sys::PluginContext> {
public:
    /**
     * Timezone module constructor. Allocates each context.
     */
    TimezoneModule(ietf::sys::PluginContext& plugin_ctx);

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
     * Get module name.
     */
    virtual constexpr const char* getName() override;

    /**
     * Timezone module destructor.
     */
    ~TimezoneModule() { }

private:
    std::shared_ptr<TimezoneOperationalContext> m_operContext;
    std::shared_ptr<TimezoneModuleChangesContext> m_changeContext;
    std::shared_ptr<TimezoneRpcContext> m_rpcContext;
};
