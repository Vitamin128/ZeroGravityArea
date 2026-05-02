#pragma once
#include"requestor.hpp"
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include<unordered_set>
#include <utility>

namespace gchrpc {
    namespace client {
        class TopicManager
        {
            public:
            using SubCallBack=std::function<void(const std::string&,const std::string&)>;
            using ptr=std::shared_ptr<TopicManager>;
            TopicManager(const Requestor::ptr& requestor):_requestor(requestor) {}

            bool createTopic(const BaseConnection::ptr& conn, const std::string& key)
            {
                LOG(NORMAL) << "创建主题: " << key << std::endl;
                return commentRequest(conn, "", TopicOptype::TOPIC_CREATE, key);
            }

            bool deleteTopic(const BaseConnection::ptr& conn, const std::string& key)
            {
                LOG(NORMAL) << "删除主题: " << key << std::endl;
                if (commentRequest(conn, "", TopicOptype::TOPIC_REMOVE, key))
                {
                    delSubscribe(key);
                    return true;
                }
                return false;
            }

            bool cancelTopic(const BaseConnection::ptr& conn, const std::string& key)
            {
                LOG(NORMAL) << "取消订阅主题: " << key << std::endl;
                if (commentRequest(conn, "", TopicOptype::TOPIC_CANCEL, key))
                {
                    delSubscribe(key);
                    return true;
                }
                return false;
            }

            bool takeTopic(const BaseConnection::ptr& conn, const std::string& key, const SubCallBack &cb)
            {
                LOG(NORMAL) << "订阅主题: " << key << std::endl;
                addSubscribe(key, cb);
                bool ret = commentRequest(conn, "", TopicOptype::TOPIC_SUBSCRIBE, key);
                if (ret == false)
                {
                    LOG(ERROR) << "订阅主题失败，移除本地回调: " << key << std::endl;
                    delSubscribe(key);
                    return false;
                }
                return true;
            }

            bool publish(const BaseConnection::ptr& conn, const std::string& msg, const std::string& key)
            {
                LOG(NORMAL) << "发布主题消息, key=" << key << std::endl;
                return commentRequest(conn, msg, TopicOptype::TOPIC_PUBLISH, key);
            }

            void onPublish(const BaseConnection::ptr& conn, const TopicRequest::ptr& req)
            {
                if (req->optype() != TopicOptype::TOPIC_PUBLISH)
                {
                    LOG(ERROR) << "onPublish 收到非 PUBLISH 类型操作, optype=" << (int)req->optype() << std::endl;
                    return;
                }
                std::string topic_key = req->topicKey();
                std::string msg = req->topicMsg();
                SubCallBack scb = getSubscribe(topic_key);
                if (scb == nullptr)
                {
                    LOG(ERROR) << "收到推送但无对应订阅回调, key=" << topic_key << std::endl;
                    return;
                }
                scb(topic_key, msg);
            }

            private:
            void addSubscribe(const std::string& key, const SubCallBack& cb)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _topic_callbacks.insert(std::make_pair(key, cb));
            }

            void delSubscribe(const std::string& key)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _topic_callbacks.erase(key);
            }

            SubCallBack getSubscribe(const std::string &key)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _topic_callbacks.find(key);
                if (it == _topic_callbacks.end())
                {
                    return SubCallBack();
                }
                return it->second;
            }

            bool commentRequest(const BaseConnection::ptr& conn, const std::string& msg,
                                const TopicOptype& optype, const std::string& key)
            {
                TopicRequest::ptr toq = MessageFactory::create<TopicRequest>();
                toq->setId(UUID::uuid());
                toq->setOptype(optype);
                toq->setTopicKey(key);
                if (optype == TopicOptype::TOPIC_PUBLISH)
                {
                    toq->setTopicMsg(msg);
                }
                BaseMessage::ptr top;
                bool ret = _requestor->send(conn, toq, top);
                if (ret == false)
                {
                    LOG(ERROR) << "主题请求发送失败, key=" << key << std::endl;
                    return false;
                }
                TopicResponse::ptr response = std::dynamic_pointer_cast<TopicResponse>(top);
                if (response == nullptr)
                {
                    LOG(ERROR) << "主题响应类型转换失败, key=" << key << std::endl;
                    return false;
                }
                if (response->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "主题请求处理失败, key=" << key << ": " << errReason(response->rcode()) << std::endl;
                    return false;
                }
                return true;
            }

            std::mutex _mutex;
            std::unordered_map<std::string, SubCallBack> _topic_callbacks;
            Requestor::ptr _requestor;
        };
    }
}