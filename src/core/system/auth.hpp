#pragma once

#include <string>
#include <list>
#include <optional>

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

    /**
     * @brief Get authorized keys.
     *
     * @return Authorized keys.
     */
    std::list<AuthorizedKey>& getKeys() const;

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
     * @brief Get iterator to the beginning.
     */
    std::list<LocalUser>::iterator begin() { return m_users.begin(); }

    /**
     * @brief Get iterator to the end.
     */
    std::list<LocalUser>::iterator end() { return m_users.end(); }

    /**
     * @brief Get local users.
     *
     * @return Local users.
     */
    std::list<LocalUser>& getUsers() const;

private:
    std::list<LocalUser> m_users;
};
}