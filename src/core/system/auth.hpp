#pragma once

#include <string>
#include <list>
#include <optional>

extern "C" {
#include <umgmt.h>
}

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
     * @brief Add user to the database.
     *
     * @param user User to add.
     */
    void addUser(LocalUser user);

    /**
     * @brief Change the password hash for the given user.
     *
     * @param name User name.
     * @param password_hash Password hash to set.
     */
    void changeUserPasswordHash(const std::string& name, const std::string& password_hash);

    /**
     * @brief Remove the password hash for the given user.
     *
     * @param name User name.
     */
    void removeUserPasswordHash(const std::string& name);

    /**
     * @brief Remove user with the given name from the database.
     *
     * @param name User name of the user to remove.
     */
    void removeUser(const std::string& name);

    /**
     * @brief Store authentication database to the system.
     */
    void storeToSystem(void);

private:
    um_db_t* m_db;
};

}
