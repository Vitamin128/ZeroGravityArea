#pragma once
#include"../common/dispatcher.hpp"
#include"requestor.hpp"
#include"rpc_caller.hpp"
#include"rpc_registry.hpp"
#include "rpc_topic.hpp"
#include <memory>

namespace gchrpc
{
    namespace client
    {
        class RegistryClient
        {
            public:
            using ptr=std::shared_ptr<RegistryClient>;
            RegistryClient(const std::string& ip,const int& port):
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _client(Clientfactory::create(ip,port)),
            _provider(std::make_shared<Provider>(_requestor))
            {
                auto req=std::bind(&Requestor::onResponse,_requestor.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_SERVICE,req);
                auto dis=std::bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                _client->setMessageCallback(dis);
                _client->connect();
            }
            bool RegistryProvider(const std::string &method, const Address &host)
            {
                return _provider->registrymethod(_client->connection(),method,host);
            }
            private:
            Requestor::ptr _requestor;
            Dispatcher::ptr _dispatcher;
            BaseClient::ptr _client;
            Provider::ptr _provider;
        };

        class DiscoveryClient
        {
            public:
            using ptr=std::shared_ptr<DiscoveryClient>;
            DiscoveryClient(const std::string& ip,const int& port,const Discoverer::offlinefunction& offFunction)
            :_requestor(std::make_shared<Requestor>()),
            _discover(std::make_shared<Discoverer>(offFunction,_requestor)),
            _client(Clientfactory::create(ip,port)),
            _dispatcher(std::make_shared<Dispatcher>())
            {
                //success这个MType::REQ_SERVICE一般用于注册服务端对客户端进行上线或下线的通知
                auto req=bind(&Discoverer::onServiceRequest,_discover.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<ServiceRequest>(MType::REQ_SERVICE,req);


                //success当服务端发送一个服务发现请求后，这里的Requestor将会接收到注册中心返回的请求，并将服务的地址保存起来
                auto rep=bind(&Requestor::onResponse,_requestor.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_SERVICE,rep);

                auto dis=bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                _client->setMessageCallback(dis);
                _client->connect();
            }

            bool ServiceDiscover(const std::string& method,Address& host)
            {
                //success讲host设置为method对应的服务，如果没有发现method，则向服务注册中心发送一个注册请求
                return _discover->ServiceDiscover(_client->connection(),method,host);
            }
            private:
            Requestor::ptr _requestor;
            Discoverer::ptr _discover;  //success存储了客户端已经发现的服务
            // Requestor::ptr _requestor;
            BaseClient::ptr _client;
            Dispatcher::ptr _dispatcher;
        };

        class RpcClient
        {
            public:
            using ptr=std::shared_ptr<RpcClient>;
            RpcClient(bool enableDiscovery,const std::string& ip,const int& port):
            _enableDiscovery(enableDiscovery),
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _caller(std::make_shared<RpcCaller>(_requestor))
            {

                //success这里是等_caller调用完成之后,清空发送的描述信息
                auto rsp_cb=std::bind(&Requestor::onResponse,_requestor.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_RPC,rsp_cb);

                if(enableDiscovery)
                {
                    auto offline=std::bind(&RpcClient::DelClient,this,std::placeholders::_1);
                    _discovery_client=std::make_shared<DiscoveryClient>(ip,port,offline);
                }
                else
                {
                    auto message_dis=std::bind(&Dispatcher::OnMessage,_dispatcher.get()
                    ,std::placeholders::_1,std::placeholders::_2);
                    _rpc_client=Clientfactory::create(ip,port);
                    _rpc_client->setMessageCallback(message_dis);
                    _rpc_client->connect();
                }
            }

            //success这里的_caller会根据服务对应的客户端的connection将消息发送出去,并且同一由同一个Requestor来处理回复
            bool call(const std::string& method,const Json::Value& params,Json::Value& result)
            {
                auto it=GetClient(method);
                if(it==nullptr)
                {
                    ELOG("获取客户端失败");
                    return false;
                }
                return _caller->call(it->connection(),method,params,result);
            }

            bool call(const std::string& method,const Json::Value& params,RpcCaller::JsonAsyncResponse& result)
            {
                auto it=GetClient(method);
                if(it==nullptr)
                {
                    ELOG("获取客户端失败");
                    return false;
                }
                return _caller->call(it->connection(),method,params,result);
            }
            bool call(const std::string& method,const Json::Value& params,RpcCaller::JsonResponseCallBack& result)
            {
                auto it=GetClient(method);
                if(it==nullptr)
                {
                    ELOG("获取客户端失败");
                    return false;
                }
                return _caller->call(it->connection(),method,params,result);
            }
            private:
            BaseClient::ptr NewClient(const Address& host)
            {
                //success这里增加的所有客户端的Rep都将在同一个_requestor中处理
                auto dis=std::bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                BaseClient::ptr newclient=Clientfactory::create(host.first,host.second);
                // PutClient(host,newclient);
                newclient->setMessageCallback(dis);
                newclient->connect();
                PutClient(host,newclient);
                return newclient;
            }
            BaseClient::ptr GetClient(const Address& host)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                auto it=_rpc_clients.find(host);
                if(it!=_rpc_clients.end())
                {
                    return it->second;
                }
                return nullptr;
            }
            BaseClient::ptr GetClient(const std::string& method)
            {
                Address host;
                BaseClient::ptr retclient;
                if(_enableDiscovery)
                {
                    bool ret=_discovery_client->ServiceDiscover(method,host);
                    //success这里如果没有发现服务，则返回nullptr
                    if(ret==false)
                    {
                        ELOG("无该方法的服务");
                        return nullptr;
                    }
                    
                    //success根据返回来的地址，获取客户端，如果没有则创建
                    retclient=GetClient(host);
                    if(retclient==nullptr)
                    {
                        ILOG("开始创建client");
                        retclient=NewClient(host);
                    }
                }
                else
                {
                    retclient=_rpc_client;
                }
                return retclient;
            }
            void PutClient(const Address& host,const BaseClient::ptr& client)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _rpc_clients.insert(std::make_pair(host,client));
            }


            void DelClient(const Address& host)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _rpc_clients.erase(host);
            }


            private:
            struct AddHash
            {
                size_t operator()(const Address& addr) const
                {
                    std::string AddString=addr.first+std::to_string(addr.second);
                    return std::hash<std::string>{}(AddString);
                }
            };
            bool _enableDiscovery;
            DiscoveryClient::ptr _discovery_client;
            Requestor::ptr _requestor;
            RpcCaller::ptr _caller;
            Dispatcher::ptr _dispatcher;
            BaseClient::ptr _rpc_client;
            std::mutex _mutex;
            std::unordered_map<Address,BaseClient::ptr,AddHash> _rpc_clients; //success服务端的地址对应客户端的指针
        };

        class TopicClient
        {
            public:
            TopicClient(const std::string& ip,const int& port):
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _topics(std::make_shared<TopicManager>(_requestor))
            {
                auto it1=std::bind(&Requestor::onResponse,_requestor.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<TopicResponse>(MType::RSP_TOPIC,it1);

                auto it2=std::bind(&TopicManager::onPublish,_topics.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<TopicRequest>(MType::REQ_TOPIC, it2);

                auto it3=std::bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                _client=Clientfactory::create(ip,port);
                _client->setMessageCallback(it3);
                _client->connect();
            }
            bool Create(const std::string& key)
            {
                return _topics->createTopic(_client->connection(),key);
            }
            bool Delete(const std::string& key)
            {
                return _topics->deleteTopic(_client->connection(),key);
            }
            bool Cancel(const std::string& key)
            {
                return _topics->cancelTopic(_client->connection(),key);
            }
            bool Subscribe(const std::string& key,const TopicManager::SubCallBack& scb)
            {
                return _topics->takeTopic(_client->connection(), key, scb);
            }
            bool Publish(const std::string& key,const std::string& msg)
            {
                return _topics->publish(_client->connection(),msg,key);
            }
            void Shutdown()
            {
                _client->shutdown();
            }
            private:
            Requestor::ptr _requestor;
            Dispatcher::ptr _dispatcher;
            BaseClient::ptr _client;
            TopicManager::ptr _topics;
        };
        
    }
}