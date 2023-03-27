#pragma once

#include <string>

namespace ietf::sys {
namespace API {
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