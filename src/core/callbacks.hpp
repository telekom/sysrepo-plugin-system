#pragma once

#include <string>
#include <sysrepo-cpp/Session.hpp>

/**
 * @brief Operational callback struct.
 */
struct OperationalCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::OperGetCb callback; ///< Callback function.
};

/**
 * @brief Module change callback struct.
 */
struct ModuleChangeCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::ModuleChangeCb callback; ///< Callback function.
};

/**
 * @brief RPC callback struct.
 */
struct RpcCallback {
    std::string xpath; ///< XPath of the data.
    sysrepo::RpcActionCb callback; ///< Callback function.
};
