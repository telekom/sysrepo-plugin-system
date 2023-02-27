#include "rpc.hpp"

namespace ietf::sys {
namespace sub::rpc {
    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SetCurrentDatetimeRpcCb::SetCurrentDatetimeRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode SetCurrentDatetimeRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
        sr::Event event, uint32_t requestId, ly::DataNode output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SystemRestartRpcCb::SystemRestartRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode SystemRestartRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
        sr::Event event, uint32_t requestId, ly::DataNode output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

    /**
     * sysrepo-plugin-generator: Generated default constructor.
     *
     * @param ctx Plugin RPC context.
     *
     */
    SystemShutdownRpcCb::SystemShutdownRpcCb(std::shared_ptr<ietf::sys::RpcContext> ctx) { m_ctx = ctx; }

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
    sr::ErrorCode SystemShutdownRpcCb::operator()(sr::Session session, uint32_t subscriptionId, std::string_view path, const ly::DataNode input,
        sr::Event event, uint32_t requestId, ly::DataNode output)
    {
        sr::ErrorCode error = sr::ErrorCode::Ok;
        return error;
    }

}
}