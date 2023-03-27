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
 * @brief Get system local users.
 *
 * @return Local users on the system.
 */
LocalUserList getLocalUserList()
{
    LocalUserList users;
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

    return users;
}

/**
 * @brief Get local user authorized keys.
 *
 * @param username Username.
 *
 * @return Authorized keys.
 */
AuthorizedKeyList getAuthorizedKeyList(const std::string& username)
{
    namespace fs = std::filesystem;

    AuthorizedKeyList keys;

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

    return keys;
}

}