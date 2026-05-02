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
            RegistryClient(const std::string& ip, const int& port):
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _client(Clientfactory::create(ip, port)),
            _provider(std::make_shared<Provider>(_requestor))
            {
                LOG(NORMAL) << "RegistryClient 初始化, 连接注册中心 " << ip << ":" << port << std::endl;
                auto req = std::bind(&Requestor::onResponse, _requestor.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_SERVICE, req);
                auto dis = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1, std::placeholders::_2);
                _client->setMessageCallback(dis);
                _client->connect();
                LOG(NORMAL) << "RegistryClient 连接完成" << std::endl;
            }

            bool RegistryProvider(const std::string &method, const Address &host)
            {
                LOG(NORMAL) << "注册服务提供者, method=" << method << std::endl;
                return _provider->registrymethod(_client->connection(), method, host);
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
            DiscoveryClient(const std::string& ip, const int& port, const Discoverer::offlinefunction& offFunction)
            :_requestor(std::make_shared<Requestor>()),
            _discover(std::make_shared<Discoverer>(offFunction, _requestor)),
            _client(Clientfactory::create(ip, port)),
            _dispatcher(std::make_shared<Dispatcher>())
            {
                LOG(NORMAL) << "DiscoveryClient 初始化, 连接注册中心 " << ip << ":" << port << std::endl;
                // 处理注册中心主动推送的服务上下线通知
                auto req = bind(&Discoverer::onServiceRequest, _discover.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<ServiceRequest>(MType::REQ_SERVICE, req);
                // 处理服务发现的响应
                auto rep = bind(&Requestor::onResponse, _requestor.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_SERVICE, rep);
                auto dis = bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1, std::placeholders::_2);
                _client->setMessageCallback(dis);
                _client->connect();
                LOG(NORMAL) << "DiscoveryClient 连接完成" << std::endl;
            }

            bool ServiceDiscover(const std::string& method, Address& host)
            {
                return _discover->ServiceDiscover(_client->connection(), method, host);
            }

            private:
            Requestor::ptr _requestor;
            Discoverer::ptr _discover;
            BaseClient::ptr _client;
            Dispatcher::ptr _dispatcher;
        };

        class RpcClient
        {
            public:
            using ptr=std::shared_ptr<RpcClient>;
            RpcClient(bool enableDiscovery, const std::string& ip, const int& port):
            _enableDiscovery(enableDiscovery),
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _caller(std::make_shared<RpcCaller>(_requestor))
            {
                LOG(NORMAL) << "RpcClient 初始化, enableDiscovery=" << enableDiscovery << ", addr=" << ip << ":" << port << std::endl;
                auto rsp_cb = std::bind(&Requestor::onResponse, _requestor.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<BaseMessage>(MType::RSP_RPC, rsp_cb);
                if (enableDiscovery)
                {
                    auto offline = std::bind(&RpcClient::DelClient, this, std::placeholders::_1);
                    _discovery_client = std::make_shared<DiscoveryClient>(ip, port, offline);
                }
                else
                {
                    auto message_dis = std::bind(&Dispatcher::OnMessage, _dispatcher.get(),
                    std::placeholders::_1, std::placeholders::_2);
                    _rpc_client = Clientfactory::create(ip, port);
                    _rpc_client->setMessageCallback(message_dis);
                    _rpc_client->connect();
                }
            }

            bool call(const std::string& method, const Json::Value& params, Json::Value& result)
            {
                auto it = GetClient(method);
                if (it == nullptr)
                {
                    LOG(ERROR) << "call 失败：无法获取对应服务的客户端, method=" << method << std::endl;
                    return false;
                }
                return _caller->call(it->connection(), method, params, result);
            }

            bool call(const std::string& method, const Json::Value& params, RpcCaller::JsonAsyncResponse& result)
            {
                auto it = GetClient(method);
                if (it == nullptr)
                {
                    LOG(ERROR) << "call 失败：无法获取对应服务的客户端, method=" << method << std::endl;
                    return false;
                }
                return _caller->call(it->connection(), method, params, result);
            }

            bool call(const std::string& method, const Json::Value& params, RpcCaller::JsonResponseCallBack& result)
            {
                auto it = GetClient(method);
                if (it == nullptr)
                {
                    LOG(ERROR) << "call 失败：无法获取对应服务的客户端, method=" << method << std::endl;
                    return false;
                }
                return _caller->call(it->connection(), method, params, result);
            }

            private:
            BaseClient::ptr NewClient(const Address& host)
            {
                LOG(NORMAL) << "创建新的 RPC 客户端连接, host=" << host.first << ":" << host.second << std::endl;
                auto dis = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1, std::placeholders::_2);
                BaseClient::ptr newclient = Clientfactory::create(host.first, host.second);
                newclient->setMessageCallback(dis);
                newclient->connect();
                PutClient(host, newclient);
                return newclient;
            }

            BaseClient::ptr GetClient(const Address& host)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _rpc_clients.find(host);
                if (it != _rpc_clients.end())
                {
                    return it->second;
                }
                return nullptr;
            }

            BaseClient::ptr GetClient(const std::string& method)
            {
                Address host;
                BaseClient::ptr retclient;
                if (_enableDiscovery)
                {
                    bool ret = _discovery_client->ServiceDiscover(method, host);
                    if (ret == false)
                    {
                        LOG(ERROR) << "服务发现失败，无该方法的服务, method=" << method << std::endl;
                        return nullptr;
                    }
                    retclient = GetClient(host);
                    if (retclient == nullptr)
                    {
                        LOG(NORMAL) << "本地无该服务连接，开始创建, method=" << method << std::endl;
                        retclient = NewClient(host);
                    }
                }
                else
                {
                    retclient = _rpc_client;
                }
                return retclient;
            }

            void PutClient(const Address& host, const BaseClient::ptr& client)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _rpc_clients.insert(std::make_pair(host, client));
            }

            void DelClient(const Address& host)
            {
                LOG(NORMAL) << "移除服务客户端连接, host=" << host.first << ":" << host.second << std::endl;
                std::unique_lock<std::mutex> lock(_mutex);
                _rpc_clients.erase(host);
            }

            private:
            struct AddHash
            {
                size_t operator()(const Address& addr) const
                {
                    std::string AddString = addr.first + std::to_string(addr.second);
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
            std::unordered_map<Address, BaseClient::ptr, AddHash> _rpc_clients;
        };

        class TopicClient
        {
            public:
            TopicClient(const std::string& ip, const int& port):
            _requestor(std::make_shared<Requestor>()),
            _dispatcher(std::make_shared<Dispatcher>()),
            _topics(std::make_shared<TopicManager>(_requestor))
            {
                LOG(NORMAL) << "TopicClient 初始化, 连接服务器 " << ip << ":" << port << std::endl;
                auto it1 = std::bind(&Requestor::onResponse, _requestor.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<TopicResponse>(MType::RSP_TOPIC, it1);
                auto it2 = std::bind(&TopicManager::onPublish, _topics.get(), std::placeholders::_1, std::placeholders::_2);
                _dispatcher->registerHandler<TopicRequest>(MType::REQ_TOPIC, it2);
                auto it3 = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1, std::placeholders::_2);
                _client = Clientfactory::create(ip, port);
                _client->setMessageCallback(it3);
                _client->connect();
                LOG(NORMAL) << "TopicClient 连接完成" << std::endl;
            }

            bool Create(const std::string& key) { return _topics->createTopic(_client->connection(), key); }
            bool Delete(const std::string& key) { return _topics->deleteTopic(_client->connection(), key); }
            bool Cancel(const std::string& key) { return _topics->cancelTopic(_client->connection(), key); }
            bool Subscribe(const std::string& key, const TopicManager::SubCallBack& scb) { return _topics->takeTopic(_client->connection(), key, scb); }
            bool Publish(const std::string& key, const std::string& msg) { return _topics->publish(_client->connection(), msg, key); }
            void Shutdown() { _client->shutdown(); }

            private:
            Requestor::ptr _requestor;
            Dispatcher::ptr _dispatcher;
            BaseClient::ptr _client;
            TopicManager::ptr _topics;
        };
    }
}