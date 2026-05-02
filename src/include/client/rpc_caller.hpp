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

            //success同步调用,这里面有个有意思的设置,先设置了一个异步的send,然后在获取异步的send返回值的future就成了同步的send
            bool call(const BaseConnection::ptr& conn,const std::string& method,
                      const Json::Value& params,Json::Value& result)
            {
                auto req=MessageFactory::create<RpcRequest>();
                req->setParams(params);
                req->setId(UUID::uuid());
                req->setMethod(method);
                req->setMType(MType::REQ_RPC);
                BaseMessage::ptr rep; 
                bool ret=_requestor->send(conn,req,rep);

                if(!ret)
                {
                    ELOG("同步请求成功但未获取到响应对象(rep is null)");
                    return false;
                }

                RpcResponse::ptr result_rep=std::dynamic_pointer_cast<RpcResponse>(rep);
                if(!result_rep)
                {
                    ELOG("响应类型错误，期望RpcResponse，实际类型不匹配");
                    return false;
                }
                if(result_rep->rcode()!=Rcode::RCODE_OK)
                {
                    ELOG("返回结果表示不正确,%s",errReason(result_rep->rcode()).c_str());
                    return false;
                }
                ELOG("返回完成");
                result=result_rep->result();
                return true;
                
            }

            //success发送request请求,给予一个future,返回一个异步的future
            bool call(const BaseConnection::ptr& conn,const std::string& method,
                      const Json::Value& params,JsonAsyncResponse& result)
            {
                RpcRequest::ptr rp=MessageFactory::create<RpcRequest>();
                rp->setId(UUID::uuid());
                rp->setMethod(method);
                rp->setMType(MType::REQ_RPC);
                rp->setParams(params);

                auto it=std::make_shared<std::promise<Json::Value>>();

                result=it->get_future();
                auto cb=std::bind(&RpcCaller::callback,this,it,std::placeholders::_1);
                bool ret=_requestor->send(conn,rp,cb);
                if(ret==false)
                {
                    ELOG("消息发送失败");
                    return false;
                }
                return true;
            }

            //success发送request请求,给予一个回调函数,不返回
            bool call(const BaseConnection::ptr& conn,const std::string& method,
                      const Json::Value& params,const JsonResponseCallBack& callback)
            {
                RpcRequest::ptr rp=MessageFactory::create<RpcRequest>();
                rp->setMethod(method);
                rp->setId(UUID::uuid());
                rp->setMType(MType::REQ_RPC);
                rp->setParams(params);


                auto it=std::bind(&RpcCaller::callbackfunc,this,callback,std::placeholders::_1);
                bool ret=_requestor->send(conn,rp,it);
                if(ret==false)
                {
                    ELOG("发送消息失败");
                    return false;
                }
                return true;
            }
            private:

            //success不获取Json::Value的结果,直接调用设置的回调函数
            void callbackfunc(const JsonResponseCallBack& cb,const BaseMessage::ptr& msg)
            {
                auto item=std::dynamic_pointer_cast<RpcResponse>(msg);
                if(!item)
                {
                    ELOG("消息类型向下转换失败");
                    return;
                }
                if(item->rcode()!=Rcode::RCODE_OK)
                {
                    ELOG("请求出错,错误信息为:%s",errReason(item->rcode()).c_str());
                    return;
                }
                cb(item->result());
            }
            //success异步获取服务端返回的msg中的Json::Value
            void callback(std::shared_ptr<std::promise<Json::Value>> result,const BaseMessage::ptr& msg)
            {
                auto item=std::dynamic_pointer_cast<RpcResponse>(msg);
                if(!item)
                {
                    ELOG("消息类型向下转换失败");
                    return;
                }
                if(item->rcode()!=Rcode::RCODE_OK)
                {
                    ELOG("请求出错,错误信息为:%s",errReason(item->rcode()).c_str());
                    return;
                }
                result->set_value(item->result());
            }
            Requestor::ptr _requestor;
        };
    }
}