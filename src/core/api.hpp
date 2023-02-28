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
         * @brief Get system hostname.
         *
         * @return Hostname.
         */
        static std::string getHostname();

        /**
         * @brief Get system timezone name from /etc/localtime.
         *
         * @return Timezone name.
         */
        static std::string getTimezoneName();
    };

    /**
     * @brief System state container API.
     */
    class SystemState {
    public:
        /**
         * @brief Get platform information.
         *
         * @return Platform information.
         */
        static PlatformInfo getPlatformInfo();

        /**
         * @brief Get clock information.
         *
         * @return Clock information.
         */
        static ClockInfo getClockInfo();
    };
}
}