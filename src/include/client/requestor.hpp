#pragma once
#include"../common/message.hpp"
#include"../common/net.hpp"
#include<future>
#include<functional>

namespace gchrpc
{
    namespace client
    {
        class Requestor
        {
            public:
            using ptr=std::shared_ptr<Requestor>;
            using RequestCallBack=std::function<void(const BaseMessage::ptr&)>;
            using AsyncResponse=std::future<BaseMessage::ptr>;
            struct RequestDescribe{
                using ptr=std::shared_ptr<RequestDescribe>;
                BaseMessage::ptr request;
                RType rtype;
                RequestCallBack callback;
                std::promise<BaseMessage::ptr> response;
            };

            //!处理response
            //success根据requestdescribe的描述来处理BaseMessage
            void onResponse(const BaseConnection::ptr& conn,BaseMessage::ptr msg)
            {
                std::string rid=msg->rid();
                RequestDescribe::ptr rd=GetRequestDescribe(rid);
                if(rd==nullptr)
                {
                    ELOG("未找到对应的RequestDescribe");
                    return;
                }
                if(rd->rtype==RType::REQ_ASYNC)
                {
                    rd->response.set_value(msg);
                }
                else if(rd->rtype==RType::REQ_CALLBACK)
                {
                    if(rd->callback)
                    {
                        rd->callback(msg);
                    }
                    else
                    {
                        ELOG("未设置回调函数");
                    }
                }
                else
                {
                    ELOG("rd的rtype的类型错误");
                }
                DeleteRequestDescribe(rid);
            }

            //!send不处理response,发送Message后将requestdescribe存储在哈希表中
            //success非阻塞无调用函数
            bool send(const BaseConnection::ptr&conn,const BaseMessage::ptr& msg,AsyncResponse& async_rsp)
            {
                RequestDescribe::ptr rd=newDescribe(msg,RType::REQ_ASYNC);
                if(rd==nullptr)
                {
                    ELOG("创建RequestDescribe失败");
                    return false;
                }
                conn->send(msg);
                async_rsp=rd->response.get_future();
                return true;
            }
        
            //success阻塞无调用函数
            bool send(const BaseConnection::ptr& conn,const BaseMessage::ptr& msg,BaseMessage::ptr& rep)
            {
                AsyncResponse AR;
                bool ret=send(conn,msg,AR);
                if(ret==false)
                {
                    ELOG("发送失败");
                    return false;
                }
                rep=AR.get();
                return true;
            }

            //success非阻塞有调用函数
            bool send(const BaseConnection::ptr& conn,const BaseMessage::ptr& msg,const RequestCallBack& rc)
            {
                RequestDescribe::ptr rd=newDescribe(msg,RType::REQ_CALLBACK,rc);
                if(rd==nullptr)
                {
                    ELOG("RequestDescribe创建失败");
                    return false;
                }
                conn->send(msg);
                return true;
            }
            private:

            //success:添加对于request的描述,描述包含promise回复,和唯一标识符rid,和回调函数
            RequestDescribe::ptr newDescribe(const BaseMessage::ptr& mp,RType Rtype,
            const RequestCallBack& rb=RequestCallBack())
            {
                std::unique_lock<std::mutex> lock(_mutex);
                RequestDescribe::ptr rp=std::make_shared<RequestDescribe>();
                rp->rtype=Rtype;
                rp->request=mp;
                if(rp->rtype==RType::REQ_CALLBACK&&rb)
                {
                    rp->callback=rb;
                }
                _request_desc.insert(std::make_pair(rp->request->rid(),rp));
                return rp;
            }

            //success用string rid获取一个request描述
            RequestDescribe::ptr GetRequestDescribe(const std::string& rid)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it=_request_desc.find(rid);
                if(it==_request_desc.end())
                {
                    DLOG("不存在对应的rid:%s",rid.c_str());
                    return nullptr;
                }
                return it->second;
            }

            //success用string rid删除一个request描述
            void DeleteRequestDescribe(const std::string& rid)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _request_desc.erase(rid);
            }

            std::mutex _mutex;
            std::unordered_map<std::string,RequestDescribe::ptr> _request_desc;
        };
    }
}