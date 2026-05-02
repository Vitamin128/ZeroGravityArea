#pragma once
#include"requestor.hpp"

namespace gchrpc 
{
    namespace client
    {
        class RpcCaller
        {
            public:
            using ptr=std::shared_ptr<RpcCaller>;
            using JsonAsyncResponse=std::future<Json::Value>;
            using JsonResponseCallBack=std::function<void(const Json::Value&)>;
            RpcCaller(const Requestor::ptr& requestor):_requestor(requestor){}

            // 同步 RPC 调用，阻塞等待结果
            bool call(const BaseConnection::ptr& conn, const std::string& method,
                      const Json::Value& params, Json::Value& result)
            {
                auto req = MessageFactory::create<RpcRequest>();
                req->setParams(params);
                req->setId(UUID::uuid());
                req->setMethod(method);
                req->setMType(MType::REQ_RPC);
                LOG(NORMAL) << "发起同步 RPC 调用, method=" << method << ", rid=" << req->rid() << std::endl;
                BaseMessage::ptr rep;
                bool ret = _requestor->send(conn, req, rep);
                if (!ret)
                {
                    LOG(ERROR) << "同步 RPC 发送失败, method=" << method << std::endl;
                    return false;
                }
                RpcResponse::ptr result_rep = std::dynamic_pointer_cast<RpcResponse>(rep);
                if (!result_rep)
                {
                    LOG(ERROR) << "响应类型转换失败，期望 RpcResponse, method=" << method << std::endl;
                    return false;
                }
                if (result_rep->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "RPC 调用失败: " << errReason(result_rep->rcode()) << std::endl;
                    return false;
                }
                LOG(NORMAL) << "同步 RPC 调用完成, method=" << method << std::endl;
                result = result_rep->result();
                return true;
            }

            // 异步 RPC 调用，返回 future 对象
            bool call(const BaseConnection::ptr& conn, const std::string& method,
                      const Json::Value& params, JsonAsyncResponse& result)
            {
                RpcRequest::ptr rp = MessageFactory::create<RpcRequest>();
                rp->setId(UUID::uuid());
                rp->setMethod(method);
                rp->setMType(MType::REQ_RPC);
                rp->setParams(params);
                LOG(NORMAL) << "发起异步 RPC 调用, method=" << method << ", rid=" << rp->rid() << std::endl;
                auto it = std::make_shared<std::promise<Json::Value>>();
                result = it->get_future();
                auto cb = std::bind(&RpcCaller::callback, this, it, std::placeholders::_1);
                bool ret = _requestor->send(conn, rp, cb);
                if (ret == false)
                {
                    LOG(ERROR) << "异步 RPC 发送失败, method=" << method << std::endl;
                    return false;
                }
                return true;
            }

            // 带回调函数的 RPC 调用
            bool call(const BaseConnection::ptr& conn, const std::string& method,
                      const Json::Value& params, const JsonResponseCallBack& callback)
            {
                RpcRequest::ptr rp = MessageFactory::create<RpcRequest>();
                rp->setMethod(method);
                rp->setId(UUID::uuid());
                rp->setMType(MType::REQ_RPC);
                rp->setParams(params);
                LOG(NORMAL) << "发起回调 RPC 调用, method=" << method << ", rid=" << rp->rid() << std::endl;
                auto it = std::bind(&RpcCaller::callbackfunc, this, callback, std::placeholders::_1);
                bool ret = _requestor->send(conn, rp, it);
                if (ret == false)
                {
                    LOG(ERROR) << "回调 RPC 发送失败, method=" << method << std::endl;
                    return false;
                }
                return true;
            }
            private:

            // 不获取 Json::Value，直接调用用户设置的回调函数
            void callbackfunc(const JsonResponseCallBack& cb, const BaseMessage::ptr& msg)
            {
                auto item = std::dynamic_pointer_cast<RpcResponse>(msg);
                if (!item)
                {
                    LOG(ERROR) << "消息类型向下转换失败" << std::endl;
                    return;
                }
                if (item->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "RPC 回调错误: " << errReason(item->rcode()) << std::endl;
                    return;
                }
                cb(item->result());
            }

            // 异步获取服务端返回的 Json::Value 并设置到 promise
            void callback(std::shared_ptr<std::promise<Json::Value>> result, const BaseMessage::ptr& msg)
            {
                auto item = std::dynamic_pointer_cast<RpcResponse>(msg);
                if (!item)
                {
                    LOG(ERROR) << "消息类型向下转换失败" << std::endl;
                    return;
                }
                if (item->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "RPC 异步回调错误: " << errReason(item->rcode()) << std::endl;
                    return;
                }
                result->set_value(item->result());
            }
            Requestor::ptr _requestor;
        };
    }
}