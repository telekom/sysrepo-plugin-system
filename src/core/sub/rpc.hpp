#pragma once

#include "core/context.hpp"

#include <iostream>
#include <optional>
#include <string_view>

#include <sysrepo-cpp/Session.hpp>
#include <libyang-cpp/Context.hpp>

namespace sr = sysrepo;
namespace ly = libyang;

namespace ietf::sys {
namespace sub::rpc {
    /**
     * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:set-current-datetime.
     */
    class SetCurrentDatetimeRpcCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin RPC context.
         *
         */
        SetCurrentDatetimeRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:set-current-datetime.
         *
         * A callback for RPC/action subscriptions.
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param path Path identifying the RPC/action.
         * @param input Data tree specifying the input of the RPC/action.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
         * Points to the operation root node.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
            uint32_t requestId, ly::DataNode output);

    private:
        std::shared_ptr<ietf::sys::RpcContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:system-restart.
     */
    class SystemRestartRpcCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin RPC context.
         *
         */
        SystemRestartRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-restart.
         *
         * A callback for RPC/action subscriptions.
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param path Path identifying the RPC/action.
         * @param input Data tree specifying the input of the RPC/action.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
         * Points to the operation root node.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
            uint32_t requestId, ly::DataNode output);

    private:
        std::shared_ptr<ietf::sys::RpcContext> m_ctx;
    };

    /**
     * @brief sysrepo-plugin-generator: Generated RPC functor for path /ietf-system:system-shutdown.
     */
    class SystemShutdownRpcCb {
    public:
        /**
         * sysrepo-plugin-generator: Generated default constructor.
         *
         * @param ctx Plugin RPC context.
         *
         */
        SystemShutdownRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx);

        /**
         * sysrepo-plugin-generator: Generated RPC/action operator() for path /ietf-system:system-shutdown.
         *
         * A callback for RPC/action subscriptions.
         * @param session An implicit session for the callback.
         * @param subscriptionId ID the subscription associated with the callback.
         * @param path Path identifying the RPC/action.
         * @param input Data tree specifying the input of the RPC/action.
         * @param requestId Request ID unique for the specific module_name. Connected events for one request (SR_EV_CHANGE and
         * @param output A handle to a tree. The callback is supposed to fill this tree with output data (if there are any).
         * Points to the operation root node.
         *
         * @return Error code.
         *
         */
        sr::ErrorCode operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input, sr::Event event,
            uint32_t requestId, ly::DataNode output);

    private:
        std::shared_ptr<ietf::sys::RpcContext> m_ctx;
    };

}
}