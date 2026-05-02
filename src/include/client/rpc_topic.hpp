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
            TopicManager(const Requestor::ptr& requestor):_requestor(requestor)
            {}
            bool createTopic(const BaseConnection::ptr& conn,const std::string& key)
            {
                return commentRequest(conn,"",TopicOptype::TOPIC_CREATE,key);
            }
            bool deleteTopic(const BaseConnection::ptr& conn,const std::string& key)
            {
                // delSubscribe(key);
                if(commentRequest(conn, "",TopicOptype::TOPIC_REMOVE,key))
                {
                    delSubscribe(key);
                    return true;
                }
                return false;
            }
            bool cancelTopic(const BaseConnection::ptr& conn,const std::string& key)
            {
                // delSubscribe(key);
                if(commentRequest(conn, "",TopicOptype::TOPIC_CANCEL,key))
                {
                    delSubscribe(key);
                    return true;
                }
                return false;
            }
            bool takeTopic(const BaseConnection::ptr& conn,const std::string& key,const SubCallBack &cb)
            {

                addSubscribe(key,cb);
                bool ret= commentRequest(conn, "",TopicOptype::TOPIC_SUBSCRIBE,key);
                if(ret==false)
                {
                    delSubscribe(key);
                    return false;
                }
                // addSubscribe(key,cb);
                return true;
            }
            bool publish(const BaseConnection::ptr& conn,const std::string& msg,const std::string& key)
            {
                return commentRequest(conn,msg,TopicOptype::TOPIC_PUBLISH,key);
            }
            void onPublish(const BaseConnection::ptr& conn,const TopicRequest::ptr& req)
            {
                if(req->optype()!=TopicOptype::TOPIC_PUBLISH)
                {
                    ELOG("操作类型错误");
                    return;
                }

                std::string topic_key=req->topicKey();
                std::string msg=req->topicMsg();
                SubCallBack scb=getSubscribe(topic_key);
                if(scb==nullptr)
                {
                    ELOG( "主题类型无法处理%s",topic_key.c_str());
                    return;
                }
                scb(topic_key,msg);

            }
            private:
            void addSubscribe(const std::string&key,const SubCallBack& cb)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _topic_callbacks.insert(std::make_pair(key,cb));
            }
            void delSubscribe(const std::string& key)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _topic_callbacks.erase(key);
            }
            SubCallBack getSubscribe(const std::string & key)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                auto it=_topic_callbacks.find(key);
                if(it==_topic_callbacks.end())
                {
                    return SubCallBack();
                }
                return it->second;
            }
            bool commentRequest(const BaseConnection::ptr& conn,const std::string& msg,const TopicOptype& optype,const std::string& key)
            {
                TopicRequest::ptr toq=MessageFactory::create<TopicRequest>();
                toq->setId(UUID::uuid());
                toq->setOptype(optype);
                toq->setTopicKey(key);
                if(optype==TopicOptype::TOPIC_PUBLISH)
                {
                    toq->setTopicMsg(msg);
                }
                BaseMessage::ptr top;
                bool ret=_requestor->send(conn,toq,top);
                if(ret==false)
                {
                    ELOG("传递请求失败");
                    return false;
                }

                TopicResponse::ptr response=std::dynamic_pointer_cast<TopicResponse>(top);
                if(response==nullptr)
                {
                    ELOG("向下转化类型失败");
                    return false;
                }

                if(response->rcode()!=Rcode::RCODE_OK)
                {
                    ELOG("请求处理失败：%s",errReason(response->rcode()).c_str());
                    return false;
                }

                return true;
                
                
                
            }
            std::mutex _mutex;
            std::unordered_map<std::string,SubCallBack>_topic_callbacks;
            Requestor::ptr _requestor;
        };
    }
}