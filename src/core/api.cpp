#include "api.hpp"
#include "core/context.hpp"
#include "core/common.hpp"
#include "core/types.hpp"

// Platform information
#include <stdexcept>
#include <memory>
#include <sys/sysinfo.h>
#include <sys/utsname.h>

// system() API
#include <cstdlib>
#include <stdexcept>
#include <cstring>

// sethostname() and gethostname()
#include <unistd.h>

#include <filesystem>
#include <fstream>

extern "C" {
// use umgmt C API for auth module
#include <umgmt.h>
}

namespace ietf::sys {
namespace API {

    /**
     * @brief Get system local users.
     *
     * @return Local users on the system.
     */
    LocalUserList System::getLocalUserList()
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
    AuthorizedKeyList System::getAuthorizedKeyList(const std::string& username)
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

    /**
     * @brief Set system current datetime. Throws a runtime_error if unable to set datetime.
     *
     * @param datetime Datetime.
     */
    void RPC::setCurrentDatetime(const std::string& datetime)
    {
        struct tm t = { 0 };
        time_t time_to_set = 0;
        struct timespec stime = { 0 };

        if (strptime(datetime.c_str(), "%FT%TZ", &t) == 0) {
            throw std::runtime_error("Failed to parse datetime");
        }

        time_to_set = mktime(&t);
        if (time_to_set == -1) {
            throw std::runtime_error("Failed to convert datetime to time_t");
        }

        stime.tv_sec = time_to_set;

        if (clock_settime(CLOCK_REALTIME, &stime) == -1) {
            throw std::runtime_error("Failed to set system time");
        }
    }

    /**
     * @brief Restart system.
     */
    void RPC::restartSystem()
    {
        if (auto rc = std::system("reboot"); rc != 0) {
            throw std::runtime_error("Failed to restart system.");
        }
    }

    /**
     * @brief Shutdown system.
     */
    void RPC::shutdownSystem()
    {
        if (auto rc = std::system("poweroff"); rc != 0) {
            throw std::runtime_error("Failed to shutdown system.");
        }
    }
}
}