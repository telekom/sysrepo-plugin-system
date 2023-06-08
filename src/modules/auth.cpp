#include "auth.hpp"

#include "core/common.hpp"

#include <memory>
#include <optional>
#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <cstring>

// use sysrepo logging api
#include <sysrepo.h>

namespace ietf::sys::auth {

/**
 * @brief Construct a new Authorized Key List object.
 */
AuthorizedKeyList::AuthorizedKeyList() { }

/**
 * @brief Load authorized keys from the system.
 *
 * @param username Username of the keys owner.
 */
void AuthorizedKeyList::loadFromSystem(const std::string& username)
{
    namespace fs = std::filesystem;

    // store to temp list in case of exception
    std::list<AuthorizedKey> keys;

    fs::path ssh_dir;

    if (username == "root") {
        ssh_dir = fs::path("/root/.ssh");
    } else {
        ssh_dir = fs::path("/home/" + username + "/.ssh");
    }

    for (const auto& entry : fs::directory_iterator(ssh_dir)) {
        if (fs::is_regular_file(entry.path()) && entry.path().extension() == ".pub") {
            std::ifstream file(entry.path());
            std::string algorithm, data;

            if (file.is_open()) {
                // read algorithm and key-data
                file >> algorithm >> data;
                keys.push_back(AuthorizedKey { .Name = entry.path().filename(), .Algorithm = algorithm, .Data = data });
            } else {
                throw std::runtime_error("Failed to open authorized key file.");
            }

            file.close();
        }
    }

    m_keys = keys;
}

/**
 * @brief Store authorized keys to the system.
 *
 * @param username Username of the keys owner.
 */
void AuthorizedKeyList::storeToSystem(const std::string& username)
{
    namespace fs = std::filesystem;

    fs::path ssh_dir;

    if (username == "root") {
        ssh_dir = fs::path("/root/.ssh");
    } else {
        ssh_dir = fs::path("/home/" + username + "/.ssh");
    }

    // create .ssh directory if not exists
    if (!fs::exists(ssh_dir)) {
        fs::create_directory(ssh_dir);
    }

    for (const auto& key : m_keys) {
        std::ofstream file(ssh_dir / key.Name / ".pub");

        if (file.is_open()) {
            // write algorithm and key-data
            file << key.Algorithm << " " << key.Data;
        } else {
            throw std::runtime_error("Failed to open authorized key file.");
        }

        file.close();
    }
}

/**
 * @brief Construct a new Local User List object.
 */
LocalUserList::LocalUserList() { }

/**
 * @brief Load local users from the system.
 */
void LocalUserList::loadFromSystem()
{
    std::list<LocalUser> users;

    um_db_t* db = nullptr;

    int rc = 0;

    db = um_db_new();
    if (db == nullptr) {
        throw std::runtime_error("Failed to initialize database.");
    }

    if (rc = um_db_load(db); rc != 0) {
        throw std::runtime_error("Failed to load user database.");
    }

    auto user_iter = um_db_get_user_list_head(db);
    while (user_iter) {
        // add local user
        const um_user_t* user = user_iter->user;

        if (um_user_get_uid(user) == 0 || (um_user_get_uid(user) >= 1000 && um_user_get_uid(user) < 65534)) {
            LocalUser temp_user;

            temp_user.Name = (char*)um_user_get_name(user);

            if (um_user_get_password_hash(user) && strcmp(um_user_get_password_hash(user), "*") && strcmp(um_user_get_password_hash(user), "!")) {
                temp_user.Password = (char*)um_user_get_password_hash(user);
            }

            users.push_back(temp_user);
        }
        user_iter = const_cast<um_user_element_t*>(user_iter->next);
    }

    if (db) {
        um_db_free(db);
    }

    m_users = users;
}

/**
 * @brief Store local users to the system.
 */
void LocalUserList::storeToSystem()
{
    um_db_t* db = nullptr;

    int rc = 0;

    db = um_db_new();
    if (db == nullptr) {
        throw std::runtime_error("Failed to initialize database.");
    }

    if (rc = um_db_load(db); rc != 0) {
        throw std::runtime_error("Failed to load user database.");
    }

    // remove all local users
    auto user_iter = um_db_get_user_list_head(db);
    while (user_iter) {
        const um_user_t* user = user_iter->user;

        if (um_user_get_uid(user) == 0 || (um_user_get_uid(user) >= 1000 && um_user_get_uid(user) < 65534)) {
            if (rc = um_db_delete_user(db, um_user_get_name(user)); rc != 0) {
                throw std::runtime_error("Failed to remove user from database.");
            }
        }

        user_iter = const_cast<um_user_element_t*>(user_iter->next);
    }

    // add local users
    for (const auto& user : m_users) {
        um_user_t* new_user = um_user_new();

        if (new_user == nullptr) {
            throw std::runtime_error("Failed to create new user.");
        }

        if (rc = um_user_set_name(new_user, user.Name.c_str()); rc != 0) {
            throw std::runtime_error("Failed to set user name.");
        }

        if (user.Password.has_value()) {
            if (rc = um_user_set_password_hash(new_user, user.Password.value_or("").c_str()); rc != 0) {
                throw std::runtime_error("Failed to set user password hash.");
            }
        }

        if (rc = um_db_add_user(db, new_user); rc != 0) {
            throw std::runtime_error("Failed to add user to database.");
        }
    }

    if (rc = um_db_store(db); rc != 0) {
        throw std::runtime_error("Failed to save user database.");
    }

    if (db) {
        um_db_free(db);
    }
}

/**
 * @brief Add new user.
 *
 * @param name User name.
 * @param password User password.
 */
void LocalUserList::addUser(const std::string& name, std::optional<std::string> password)
{
    LocalUser user;

    user.Name = name;
    user.Password = password;

    m_users.push_back(user);
}

/**
 * @brief Change user password to a new value.
 *
 * @param name User name.
 * @param password Password to set.
 */
void LocalUserList::changeUserPassword(const std::string& name, std::string password)
{
    for (auto& user : m_users) {
        if (user.Name == name) {
            user.Password = password;
        }
    }
}

DatabaseContext::DatabaseContext()
    : m_db(nullptr)
{
}

/**
 * @brief Load authentication database from the system.
 */
void DatabaseContext::loadFromSystem(void)
{
    // load only modified users since they
}

/**
 * @brief Add user to the database.
 *
 * @param user User to add.
 */
void DatabaseContext::addUser(LocalUser user) { m_new_users.addUser(user.Name, std::nullopt); }

/**
 * @brief Change the password hash for the given user.
 *
 * @param name User name.
 * @param password_hash Password hash to set.
 */
void DatabaseContext::changeUserPasswordHash(const std::string& name, const std::string& password_hash)
{
    m_mod_users.changeUserPassword(name, password_hash);
}

/**
 * @brief Remove the password hash for the given user.
 *
 * @param name User name.
 */
void DatabaseContext::removeUserPasswordHash(const std::string& name)
{
    um_user_t* user = nullptr;

    user = um_db_get_user(m_db, name.c_str());
    if (user) {
        if (int rc = um_user_set_password_hash(user, nullptr); rc != 0) {
            throw std::runtime_error("Unable to set user password hash.");
        }
    } else {
        throw std::runtime_error("Unable to find the user with the given name.");
    }
}

/**
 * @brief Remove user with the given name from the database.
 *
 * @param name User name of the user to remove.
 */
void DatabaseContext::removeUser(const std::string& name)
{
    if (int rc = um_db_delete_user(m_db, name.c_str()); rc != 0) {
        throw std::runtime_error("Unable to remove given user from the database.");
    }
}

/**
 * @brief Store authentication database to the system.
 */
void storeToSystem(void);

/**
 * @brief Store authentication database to the system.
 */
void DatabaseContext::storeToSystem(void)
{
    if (!m_db) {
        throw std::runtime_error("Database uninitialized.");
    }

    if (int rc = um_db_store(m_db); rc != 0) {
        throw std::runtime_error("Unable to store auth database.");
    }
}

}

namespace ietf::sys::sub::change {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
AuthUserAuthenticationOrderModuleChangeCb::AuthUserAuthenticationOrderModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx) { m_ctx = ctx; }

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
sr::ErrorCode AuthUserAuthenticationOrderModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
AuthUserModuleChangeCb::AuthUserModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx) { m_ctx = ctx; }

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
sr::ErrorCode AuthUserModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    // create database context
    auth::DatabaseContext db;

    switch (event) {
    case sysrepo::Event::Change:
        // apply user changes to the database context
        for (auto& change : session.getChanges("/ietf-system:system/authentication/user/name")) {
            SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

            SRPLG_LOG_DBG(
                ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());
        }

        // apply password changes to the database context
        for (auto& change : session.getChanges("/ietf-system:system/authentication/user/password")) {
            SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.path().c_str());
            SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

            SRPLG_LOG_DBG(
                ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());
        }

        // apply database context to the system
        break;
    default:
        break;
    }

    return sr::ErrorCode::CallbackFailed;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin module change context.
 *
 */
AuthUserAuthorizedKeyModuleChangeCb::AuthUserAuthorizedKeyModuleChangeCb(std::shared_ptr<AuthModuleChangesContext> ctx) { m_ctx = ctx; }

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
sr::ErrorCode AuthUserAuthorizedKeyModuleChangeCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, sr::Event event, uint32_t requestId)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    switch (event) {
    case sysrepo::Event::Change:
        for (auto& change : session.getChanges(subXPath->data())) {
            SRPLG_LOG_DBG(ietf::sys::PLUGIN_NAME, "Value of %s modified.", change.node.schema().name().data());

            SRPLG_LOG_DBG(
                ietf::sys::PLUGIN_NAME, "\n%s", change.node.printStr(libyang::DataFormat::XML, libyang::PrintFlags::WithDefaultsAll)->data());

            SRPLG_LOG_DBG(PLUGIN_NAME, "Node path: %s", change.node.path().data());

            const auto& user_name = ietf::sys::extractListKeyFromXPath("user", "name", change.node.path());

            SRPLG_LOG_DBG(PLUGIN_NAME, "Username for authorized key: %s", user_name.data());

            switch (change.operation) {
            case sysrepo::ChangeOperation::Created:
            case sysrepo::ChangeOperation::Modified: {
                break;
            }
            case sysrepo::ChangeOperation::Deleted:
                break;
            case sysrepo::ChangeOperation::Moved:
                break;
            }
        }
        break;
    default:
        break;
    }

    return sr::ErrorCode::CallbackFailed;
}
}

namespace ietf::sys::sub::oper {
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserAuthenticationOrderOperGetCb::AuthUserAuthenticationOrderOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user-authentication-order.
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
sr::ErrorCode AuthUserAuthenticationOrderOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserNameOperGetCb::AuthUserNameOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/name.
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
sr::ErrorCode AuthUserNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserPasswordOperGetCb::AuthUserPasswordOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/password.
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
sr::ErrorCode AuthUserPasswordOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserAuthorizedKeyNameOperGetCb::AuthUserAuthorizedKeyNameOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/name.
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
sr::ErrorCode AuthUserAuthorizedKeyNameOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserAuthorizedKeyAlgorithmOperGetCb::AuthUserAuthorizedKeyAlgorithmOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/algorithm.
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
sr::ErrorCode AuthUserAuthorizedKeyAlgorithmOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserAuthorizedKeyKeyDataOperGetCb::AuthUserAuthorizedKeyKeyDataOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s']/key-data.
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
sr::ErrorCode AuthUserAuthorizedKeyKeyDataOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserAuthorizedKeyOperGetCb::AuthUserAuthorizedKeyOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s']/authorized-key[name='%s'].
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
sr::ErrorCode AuthUserAuthorizedKeyOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}

/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthUserOperGetCb::AuthUserOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication/user[name='%s'].
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
sr::ErrorCode AuthUserOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;

    auth::LocalUserList users;

    try {
        users.loadFromSystem();

        for (auto& user : users) {
            user.AuthorizedKeys = std::make_optional<auth::AuthorizedKeyList>();
            user.AuthorizedKeys->loadFromSystem(user.Name);
        }

        // create YANG subtree
        for (auto& user : users) {
            std::stringstream path_buffer;

            path_buffer << "user[name='" << user.Name << "']";

            auto user_node = output->newPath(path_buffer.str());
            if (user_node) {
                if (user.Password) {
                    user_node->newPath("password", user.Password);
                }

                // create authorized-key subtree
                if (user.AuthorizedKeys) {
                    for (auto& key : user.AuthorizedKeys.value()) {
                        std::stringstream key_path_buffer;

                        key_path_buffer << "authorized-key[name='" << key.Name << "']";

                        auto key_node = user_node->newPath(key_path_buffer.str());
                        if (key_node) {
                            key_node->newPath("algorithm", key.Algorithm);
                            key_node->newPath("key-data", key.Data);
                        } else {
                            error = sr::ErrorCode::Internal;
                            break;
                        }
                    }
                }
            } else {
                error = sr::ErrorCode::Internal;
                break;
            }
        }
    } catch (const std::exception& e) {
        SRPLG_LOG_ERR(PLUGIN_NAME, "Error loading local users");
        SRPLG_LOG_ERR(PLUGIN_NAME, "%s", e.what());
        // std::cerr << "Error loading local users: " << e.what() << std::endl;
        error = sr::ErrorCode::Internal;
    }

    return error;
}
/**
 * sysrepo-plugin-generator: Generated default constructor.
 *
 * @param ctx Plugin operational context.
 *
 */
AuthOperGetCb::AuthOperGetCb(std::shared_ptr<AuthOperationalContext> ctx) { m_ctx = ctx; }

/**
 * sysrepo-plugin-generator: Generated operator() for path /ietf-system:system/authentication.
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
sr::ErrorCode AuthOperGetCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view moduleName,
    std::optional<std::string_view> subXPath, std::optional<std::string_view> requestXPath, uint32_t requestId, std::optional<ly::DataNode>& output)
{
    sr::ErrorCode error = sr::ErrorCode::Ok;
    return error;
}
}

/**
 * Authentication module constructor. Allocates each context.
 */
AuthModule::AuthModule()
{
    m_operContext = std::make_shared<AuthOperationalContext>();
    m_changeContext = std::make_shared<AuthModuleChangesContext>();
    m_rpcContext = std::make_shared<AuthRpcContext>();
}

/**
 * Return the operational context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getOperationalContext() { return m_operContext; }

/**
 * Return the module changes context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getModuleChangesContext() { return m_changeContext; }

/**
 * Return the RPC context from the module.
 */
std::shared_ptr<IModuleContext> AuthModule::getRpcContext() { return m_rpcContext; }

/**
 * Get all operational callbacks which the module should use.
 */
std::list<OperationalCallback> AuthModule::getOperationalCallbacks()
{
    return {
        OperationalCallback { "/ietf-system:system/authentication/user", ietf::sys::sub::oper::AuthUserOperGetCb(this->m_operContext) },
    };
}

/**
 * Get all module change callbacks which the module should use.
 */
std::list<ModuleChangeCallback> AuthModule::getModuleChangeCallbacks()
{
    return {
        ModuleChangeCallback { "/ietf-system:system/authentication/user", ietf::sys::sub::change::AuthUserModuleChangeCb(this->m_changeContext) },
        ModuleChangeCallback { "/ietf-system:system/authentication/user/authorized-key",
            ietf::sys::sub::change::AuthUserAuthorizedKeyModuleChangeCb(this->m_changeContext) },
    };
}

/**
 * Get all RPC callbacks which the module should use.
 */
std::list<RpcCallback> AuthModule::getRpcCallbacks() { return {}; }

/**
 * Get module name.
 */
constexpr const char* AuthModule::getName() { return "Authentication"; };