#pragma once
#include "../common/net.hpp"
#include <unordered_map>
#include "../common/message.hpp"
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>
namespace gchrpc
{
    namespace server
    {
        class TopicManager
        {
        private:


            //success 订阅者的结构体封装baseconnection
            struct subscriber
            {
                using ptr = std::shared_ptr<subscriber>;
                std::mutex _mutex;
                BaseConnection::ptr _conn;
                // std::vector<std::string>topics;
                std::unordered_set<std::string> _topics;
                subscriber(const BaseConnection::ptr &conn) : _conn(conn) {}
                void AppendTopic(const std::string &newtopic)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _topics.insert(newtopic);
                }
                void RemoveTopic(const std::string &nowtopic)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_topics.find(nowtopic) != _topics.end())
                    {
                        _topics.erase(nowtopic);
                    }
                }
            };


            //success订阅端的结构体封装订阅名string和订阅者subscriber
            struct topic
            {
                using ptr = std::shared_ptr<topic>;
                std::mutex _mutex;
                std::string _topic_name;
                std::unordered_set<subscriber::ptr> _subscribers;
                topic(const std::string &topic_name) : _topic_name(topic_name) {}
                std::unordered_set<subscriber::ptr> GetSubscribers()
                {
                    std::unique_lock<std::mutex>lock(_mutex);
                    return _subscribers;
                }
                void AppendSubscriber(const subscriber::ptr &newsubscriber)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    _subscribers.insert(newsubscriber);
                }
                void RemoveSubscriber(const subscriber::ptr &nowsubscriber)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_subscribers.find(nowsubscriber) != _subscribers.end())
                    {
                        _subscribers.erase(nowsubscriber);
                    }
                }
                void PublishMessage(const BaseMessage::ptr &msg)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    for (auto &it : _subscribers)
                    {
                        it->_conn->send(msg);
                    }
                }
            };


        public:
            using ptr = std::shared_ptr<TopicManager>;


            //success 用于topic接收消息的接口
            void Ontopicrequest(const BaseConnection::ptr& conn,const TopicRequest::ptr& msg)
            {
                bool ret=true;
                TopicOptype topicoptype=msg->optype();
                switch (topicoptype)
                {
                case TopicOptype::TOPIC_CREATE:
                    Topiccreate(conn,msg);
                    break;
                case TopicOptype::TOPIC_CANCEL:
                    Topiccancel(conn,msg);
                    break;
                case TopicOptype::TOPIC_REMOVE:
                    Topicremove(conn,msg);
                    break;
                case TopicOptype::TOPIC_PUBLISH:
                    ret=Topicpublish(conn,msg);
                    break;
                case TopicOptype::TOPIC_SUBSCRIBE:
                    ret=Topicsubscribe(conn,msg);
                    break;
                default:
                    return Errorresponse(conn,msg,Rcode::RCODE_INVALID_OPTYPE);
                    break;
                }
                if(ret)
                {
                    Topicresponse(conn,msg);
                }
                else
                {
                    Errorresponse(conn,msg,Rcode::RCODE_NOT_FOUND_TOPIC);
                }
                // Topicresponse(conn,msg);
            }


            //success 用于topic断开订阅者的接口
            void Onshutdown(const BaseConnection::ptr& conn)
            {
                std::vector<topic::ptr> topics;
                subscriber::ptr optsubscriber;
                {
                  std::unique_lock<std::mutex> lock(_mutex);
                  auto Itsubscribe = ManagerSubscribes.find(conn);
                  if (Itsubscribe == ManagerSubscribes.end()) {
                    return;
                  }
                  optsubscriber=Itsubscribe->second;
                  ManagerSubscribes.erase(conn);
                //   optsubscriber=Itsubscribe->second;
                  for (auto &item : optsubscriber->_topics) {
                    auto it = ManagerTopics.find(item);
                    if (it != ManagerTopics.end()) {
                      topics.push_back(it->second);
                    }
                  }
                }
                for(auto& singletopic:topics)
                {
                    singletopic->RemoveSubscriber(optsubscriber);
                }
            }
        private:
        //success 错误操作回复
            void Errorresponse(const BaseConnection::ptr& conn,const TopicRequest::ptr& msg,const Rcode& rcode)
            {
                TopicResponse::ptr rep=MessageFactory::create<TopicResponse>();
                rep->setId(msg->rid());
                rep->setMType(MType::RSP_TOPIC);
                rep->setRCode(rcode);
                conn->send(rep);
            }
            //success 正确操作回复
            void Topicresponse(const BaseConnection::ptr& conn,const TopicRequest::ptr& msg)
            {
                TopicResponse::ptr rep=MessageFactory::create<TopicResponse>();
                rep->setId(msg->rid());
                rep->setMType(MType::RSP_TOPIC);
                rep->setRCode(Rcode::RCODE_OK);
                conn->send(rep);
            }
            //success 订阅创建
            void Topiccreate(const BaseConnection::ptr &conn, const TopicRequest::ptr& msg)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                auto it=std::make_shared<topic>(msg->topicKey());
                ManagerTopics.insert(std::make_pair(msg->topicKey(),it));
            }
            //success 订阅删除
            void Topicremove(const BaseConnection::ptr &conn, const TopicRequest::ptr &msg)
            {
                topic::ptr opttopic;
                // topic::ptr topic_it;
                std::unordered_set<subscriber::ptr> subscriberArr;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto topic_it = ManagerTopics.find(msg->topicKey());
                    if (topic_it == ManagerTopics.end())
                    {
                        return;
                    }
                    opttopic = topic_it->second;
                    ManagerTopics.erase(topic_it);
                }
                subscriberArr = opttopic->GetSubscribers();
                for (auto &it : subscriberArr)
                {
                    it->RemoveTopic(msg->topicKey());
                }
                // ManagerTopics.erase(msg->topicKey());
            }

            //success 订阅者发起订阅
            bool Topicsubscribe(const BaseConnection::ptr &conn, const TopicRequest::ptr &msg)
            {
                topic::ptr opttopic;
                subscriber::ptr optsub;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto topic_it = ManagerTopics.find(msg->topicKey());
                    if (topic_it == ManagerTopics.end())
                    {
                        // opttopic=std::make_shared<topic>();
                        // ManagerTopics.insert(std::make_pair(msg->topicKey(),opttopic));
                        return false;
                    }
                    opttopic = topic_it->second;
                    auto sub_it = ManagerSubscribes.find(conn);
                    if (sub_it == ManagerSubscribes.end())
                    {
                        optsub = std::make_shared<subscriber>(conn);
                        ManagerSubscribes.insert(std::make_pair(conn, optsub));
                    }
                    else
                    {
                        optsub = sub_it->second;
                    }
                }
                optsub->AppendTopic(msg->topicKey());
                opttopic->AppendSubscriber(optsub);
                return true;
            }

            //success 订阅者取消订阅
            void Topiccancel(const BaseConnection::ptr &conn, const TopicRequest::ptr &msg)
            {
                subscriber::ptr client;
                topic::ptr Topic;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it1 = ManagerTopics.find(msg->topicKey());
                    if (it1 != ManagerTopics.end())
                    {
                        Topic = it1->second;
                    }
                    auto it2 = ManagerSubscribes.find(conn);
                    if (it2 != ManagerSubscribes.end())
                    {
                        client = it2->second;
                    }
                }
                if (client)
                {
                    client->RemoveTopic(msg->topicKey());
                }
                if (client && Topic)
                {
                    Topic->RemoveSubscriber(client);
                }
            }

            //success 订阅者发布订阅消息
            bool Topicpublish(const BaseConnection::ptr &conn, const TopicRequest::ptr &msg)
            {
                topic::ptr PublishTopic;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = ManagerTopics.find(msg->topicKey());
                    if (it != ManagerTopics.end())
                    {
                        PublishTopic = it->second;
                    }
                    else
                    {
                        return false;
                    }
                }
                PublishTopic->PublishMessage(msg);
                return true;
            }
        private:
            std::mutex _mutex;
            std::unordered_map<std::string, topic::ptr> ManagerTopics;
            std::unordered_map<BaseConnection::ptr, subscriber::ptr> ManagerSubscribes;
        };
    }
}