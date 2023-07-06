#pragma once

#include "core/context.hpp"
#include <srpcpp/module.hpp>
#include <srpcpp/module-registry.hpp>

#include <sysrepo-cpp/Subscription.hpp>
#include <libyang-cpp/Context.hpp>

#include <string>
#include <list>
#include <optional>

extern "C" {
#include <umgmt.h>
}

// helpers
namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::sys::auth {

/**
 * @brief Authorized key helper struct.
 */
struct AuthorizedKey {
    std::string Name; ///< Key name.
    std::string Algorithm; ///< Key algorithm.
    std::string Data; ///< Key data.
};

/**
 * @brief Authorized key list.
 */
class AuthorizedKeyList {
public:
    /**
     * @brief Construct a new Authorized Key List object.
     */
    AuthorizedKeyList();

    /**
     * @brief Load authorized keys from the system.
     *
     * @param username Username of the keys owner.
     */
    void loadFromSystem(const std::string& username);

    /**
     * @brief Store authorized keys to the system.
     *
     * @param username Username of the keys owner.
     */
    void storeToSystem(const std::string& username);

    /**
     * @brief Get iterator to the beginning.
     */
    std::list<AuthorizedKey>::iterator begin() { return m_keys.begin(); }

    /**
     * @brief Get iterator to the end.
     */
    std::list<AuthorizedKey>::iterator end() { return m_keys.end(); }

private:
    std::list<AuthorizedKey> m_keys;
};

/**
 * @brief Local user helper struct.
 */
struct LocalUser {
    std::string Name; ///< User name.
    std::optional<std::string> Password; ///< User password hash.
    std::optional<AuthorizedKeyList> AuthorizedKeys; ///< User authorized keys.
};

/**
 * @brief Local user list.
 */
class LocalUserList {
public:
    /**
     * @brief Construct a new Local User List object.
     */
    LocalUserList();

    /**
     * @brief Load local users from the system.
     */
    void loadFromSystem();

    /**
     * @brief Store local users to the system.
     */
    void storeToSystem();

    /**
     * @brief Add new user.
     *
     * @param name User name.
     * @param password User password.
     */
    void addUser(const std::string& name, std::optional<std::string> password);

    /**
     * @brief Change user password to a new value.
     *
     * @param name User name.
     * @param password Password to set.
     */
    void changeUserPassword(const std::string& name, std::string password);

    /**
     * @brief Get iterator to the beginning.
     */
    std::list<LocalUser>::iterator begin() { return m_users.begin(); }

    /**
     * @brief Get iterator to the end.
     */
    std::list<LocalUser>::iterator end() { return m_users.end(); }

private:
    std::list<LocalUser> m_users;
};

class DatabaseContext {
public:
    DatabaseContext();

    /**
     * @brief Load authentication database from the system.
     */
    void loadFromSystem(void);

    /**
     * @brief Create user in the database. Adds the user to the list of new users.
     *
     * @param user User name of the new user.
     */
    void createUser(const std::string& name);

    /**
     * @brief Delete user with the given name from the database. Adds the user to the list of users to be deleted.
     *
     * @param name User name of the user to remove.
     */
    void deleteUser(const std::string& name);

    /**
     * @brief Modify the password hash for the given user.
     *
     * @param name User name.
     * @param password_hash Password hash to set.
     */
    void modifyUserPasswordHash(const std::string& name, const std::string& password_hash);

    /**
     * @brief Delete the password hash for the given user.
     *
     * @param name User name.
     */
    void deleteUserPasswordHash(const std::string& name);

    /**
     * @brief Store database context changes to the system.
     */
    void storeToSystem(void);

private:
    um_db_t* m_db;
};

}

/**
 * Operational context for the authentication module.
 */
class AuthOperationalContext : public srpc::IModuleContext { };

/**
 * Module changes context for the authentication module.
 */
class AuthModuleChangesContext : public srpc::IModuleContext { };

/**
 * RPC context for the authentication module.
 */
class AuthRpcContext : public srpc::IModuleContext { };

namespace ietf::sys::sub::change {
/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/authentication/user-authentication-order.
 */
class AuthUserAuthenticationOrderModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    AuthUserAuthenticationOrderModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user-authentication-order.
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
    std::shared_ptr<AuthModuleChangesContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated module change functor for path /ietf-system:system/authentication/user[name='%s'].
 */
class AuthUserModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    AuthUserModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated module change operator() for path /ietf-system:system/authentication/user[name='%s'].
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
    std::shared_ptr<AuthModuleChangesContext> m_ctx;
};

/**
 * @brief Functor for path /ietf-system:system/authentication/user/authorized-key.
 */
class AuthUserAuthorizedKeyModuleChangeCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin module change context.
     *
     */
    AuthUserAuthorizedKeyModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx);

    /**
     * Functor for path /ietf-system:system/authentication/user/authorized-key.
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
    std::shared_ptr<AuthModuleChangesContext> m_ctx;
};
}

namespace ietf::sys::sub::oper {
/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/authentication/user-authentication-order.
 */
class AuthUserAuthenticationOrderOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserAuthenticationOrderOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/authentication/user-authentication-order.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/authentication/user[name='%s']/name.
 */
class AuthUserNameOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserNameOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/authentication/user[name='%s']/name.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/authentication/user[name='%s']/password.
 */
class AuthUserPasswordOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserPasswordOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/authentication/user[name='%s']/password.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path
 * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/name.
 */
class AuthUserAuthorizedKeyNameOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserAuthorizedKeyNameOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path
     * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/name.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path
 * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm.
 */
class AuthUserAuthorizedKeyAlgorithmOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserAuthorizedKeyAlgorithmOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path
     * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path
 * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data.
 */
class AuthUserAuthorizedKeyKeyDataOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserAuthorizedKeyKeyDataOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path
     * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path
 * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s'].
 */
class AuthUserAuthorizedKeyOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserAuthorizedKeyOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path
     * /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s'].
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/authentication/user[name='%s'].
 */
class AuthUserOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthUserOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/authentication/user[name='%s'].
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};

/**
 * @brief sysrepo-plugin-generator: Generated operational get functor for path /ietf-system:system/authentication.
 */
class AuthOperGetCb {
public:
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin operational context.
     *
     */
    AuthOperGetCb(std::shared_ptr<AuthOperationalContext> ctx);

    /**
     * sysrepo-plugin-generator: Generated operational get operator() for path /ietf-system:system/authentication.
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
    std::shared_ptr<AuthOperationalContext> m_ctx;
};
}

/**
 * @brief Checker used to check if ietf-system/system/authentication/user values are contained on the system.
 */
class UserValueChecker : public srpc::DatastoreValuesChecker<ietf::sys::PluginContext> {
public:
    /**
     * @brief Default constructor.
     */
    UserValueChecker(ietf::sys::PluginContext& plugin_ctx);

    /**
     * @brief Check for the datastore values on the system.
     *
     * @param session Sysrepo session used for retreiving datastore values.
     *
     * @return Enum describing the output of values comparison.
     */
    virtual srpc::DatastoreValuesCheckStatus checkValues(sysrepo::Session& session) override;
};

/**
 * @brief Checker used to check if ietf-system/system/authentication/user/authorized-key values are contained on the system.
 */
class UserAuthorizedKeyValueChecker : public srpc::DatastoreValuesChecker<ietf::sys::PluginContext> {
public:
    /**
     * @brief Default constructor.
     */
    UserAuthorizedKeyValueChecker(ietf::sys::PluginContext& plugin_ctx);

    /**
     * @brief Check for the datastore values on the system.
     *
     * @param session Sysrepo session used for retreiving datastore values.
     *
     * @return Enum describing the output of values comparison.
     */
    virtual srpc::DatastoreValuesCheckStatus checkValues(sysrepo::Session& session) override;
};

/**
 * @brief Authentication container module.
 * @brief Provides callbacks for user list and each user authorized-key list element.
 */
class AuthModule : public srpc::IModule<ietf::sys::PluginContext> {
public:
    /**
     * Authentication module constructor. Allocates each context.
     */
    AuthModule(ietf::sys::PluginContext& plugin_ctx);

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
     * Auth module destructor.
     */
    ~AuthModule() { }

private:
    std::shared_ptr<AuthOperationalContext> m_operContext;
    std::shared_ptr<AuthModuleChangesContext> m_changeContext;
    std::shared_ptr<AuthRpcContext> m_rpcContext;
};
