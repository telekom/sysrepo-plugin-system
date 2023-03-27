#pragma once

#include "core/types.hpp"

namespace ietf::sys {
namespace API {
    /**
     * @brief System container API.
     */
    class System {
    public:
        /**
         * @brief Get system local users.
         *
         * @return Local users on the system.
         */
        static LocalUserList getLocalUserList();

        /**
         * @brief Get local user authorized keys.
         *
         * @param username Username.
         *
         * @return Authorized keys.
         */
        static AuthorizedKeyList getAuthorizedKeyList(const std::string& username);
    };

    class RPC {
    public:
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
    };
}
}