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

            // 根据 requestdescribe 的描述处理服务端返回的 BaseMessage
            void onResponse(const BaseConnection::ptr& conn, BaseMessage::ptr msg)
            {
                std::string rid = msg->rid();
                RequestDescribe::ptr rd = GetRequestDescribe(rid);
                if (rd == nullptr)
                {
                    LOG(ERROR) << "未找到对应的 RequestDescribe, rid=" << rid << std::endl;
                    return;
                }
                if (rd->rtype == RType::REQ_ASYNC)
                {
                    rd->response.set_value(msg);
                }
                else if (rd->rtype == RType::REQ_CALLBACK)
                {
                    if (rd->callback)
                    {
                        rd->callback(msg);
                    }
                    else
                    {
                        LOG(ERROR) << "未设置回调函数, rid=" << rid << std::endl;
                    }
                }
                else
                {
                    LOG(ERROR) << "RequestDescribe 的 rtype 类型错误, rid=" << rid << std::endl;
                }
                DeleteRequestDescribe(rid);
            }

            // 异步发送（无回调），通过 future 获取响应
            bool send(const BaseConnection::ptr& conn, const BaseMessage::ptr& msg, AsyncResponse& async_rsp)
            {
                RequestDescribe::ptr rd = newDescribe(msg, RType::REQ_ASYNC);
                if (rd == nullptr)
                {
                    LOG(ERROR) << "创建 RequestDescribe 失败" << std::endl;
                    return false;
                }
                conn->send(msg);
                LOG(NORMAL) << "异步请求已发送, rid=" << msg->rid() << std::endl;
                async_rsp = rd->response.get_future();
                return true;
            }

            // 同步发送（阻塞等待响应）
            bool send(const BaseConnection::ptr& conn, const BaseMessage::ptr& msg, BaseMessage::ptr& rep)
            {
                AsyncResponse AR;
                bool ret = send(conn, msg, AR);
                if (ret == false)
                {
                    LOG(ERROR) << "异步发送失败" << std::endl;
                    return false;
                }
                rep = AR.get();
                LOG(NORMAL) << "同步请求响应已获取, rid=" << msg->rid() << std::endl;
                return true;
            }

            // 异步发送（带回调函数）
            bool send(const BaseConnection::ptr& conn, const BaseMessage::ptr& msg, const RequestCallBack& rc)
            {
                RequestDescribe::ptr rd = newDescribe(msg, RType::REQ_CALLBACK, rc);
                if (rd == nullptr)
                {
                    LOG(ERROR) << "创建 RequestDescribe 失败" << std::endl;
                    return false;
                }
                conn->send(msg);
                LOG(NORMAL) << "回调请求已发送, rid=" << msg->rid() << std::endl;
                return true;
            }
            private:

            // 添加对请求的描述（包含 promise、rid 和回调函数）
            RequestDescribe::ptr newDescribe(const BaseMessage::ptr& mp, RType Rtype,
            const RequestCallBack& rb = RequestCallBack())
            {
                std::unique_lock<std::mutex> lock(_mutex);
                RequestDescribe::ptr rp = std::make_shared<RequestDescribe>();
                rp->rtype = Rtype;
                rp->request = mp;
                if (rp->rtype == RType::REQ_CALLBACK && rb)
                {
                    rp->callback = rb;
                }
                _request_desc.insert(std::make_pair(rp->request->rid(), rp));
                return rp;
            }

            // 根据 rid 查找请求描述
            RequestDescribe::ptr GetRequestDescribe(const std::string& rid)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _request_desc.find(rid);
                if (it == _request_desc.end())
                {
                    LOG(WARNING) << "不存在对应的 rid: " << rid << std::endl;
                    return nullptr;
                }
                return it->second;
            }

            // 根据 rid 删除请求描述
            void DeleteRequestDescribe(const std::string& rid)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _request_desc.erase(rid);
            }

            std::mutex _mutex;
            std::unordered_map<std::string, RequestDescribe::ptr> _request_desc;
        };
    }
}