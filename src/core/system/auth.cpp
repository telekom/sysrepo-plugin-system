#include "auth.hpp"

#include <stdexcept>
#include <filesystem>
#include <fstream>
#include <cstring>

extern "C" {
// use umgmt C API for auth module
#include <umgmt.h>
}

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

}