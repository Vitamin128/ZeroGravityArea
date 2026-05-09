#pragma once
#include"../common/dispatcher.hpp"
#include"../client/rpc_client.hpp"
#include"rpc_router.hpp"
#include"rpc_registry.hpp"
#include "rpc_topic.hpp"
#include <functional>
#include <memory>

namespace gchrpc
{
    namespace server
    {
        class RegistryServer
        {
            public:
            using ptr=std::shared_ptr<RegistryServer>;
            RegistryServer(const int& port):_dispatcher(std::make_shared<Dispatcher>()),
            _pdmanager(std::make_shared<PDManager>())
            {
                _server=Serverfactory::create(port);
                auto it1=std::bind(&PDManager::onServiceResponse,_pdmanager.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<ServiceRequest>(MType::REQ_SERVICE,it1);

                auto it2=std::bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                _server->setMessageCallback(it2);

                auto it3=std::bind(&RegistryServer::ShutDown,this,std::placeholders::_1);
                _server->setCloseCallback(it3);
            }
            void Start()
            {
                _server->start();
            }
            void ShutDown(const BaseConnection::ptr conn)
            {
                _pdmanager->onShutdown(conn);
            }
            private:
            BaseServer::ptr _server;
            Dispatcher::ptr _dispatcher;
            PDManager::ptr _pdmanager;
        };

        class RpcServer
        {
            public:
            using ptr=std::shared_ptr<RpcServer>;
            RpcServer(const Address& access_addr,bool enableRegistry=false,const Address& registry_server_addr=Address())
            :_enableRegistry(enableRegistry),
            _access_addr(access_addr),
            _router(std::make_shared<RpcRouter>()),
            _dispatcher(std::make_shared<Dispatcher>())
            {
                if(_enableRegistry)
                {
                    _reg_client=std::make_shared<client::RegistryClient>
                    (registry_server_addr.first,registry_server_addr.second);
                }

                // _router=std::make_shared<RpcRouter>();

                auto it1=std::bind(&RpcRouter::onRpcRequest,_router.get()
                ,std::placeholders::_1,std::placeholders::_2);

                _dispatcher->registerHandler<RpcRequest>(MType::REQ_RPC,it1);
                auto it2=std::bind(&Dispatcher::OnMessage,_dispatcher.get()
                ,std::placeholders::_1,std::placeholders::_2);

                _server=Serverfactory::create(access_addr.second);
                _server->setMessageCallback(it2);

            }
            void RegistryMethod(const ServiceDescribe::ptr& service )
            {
                if(_enableRegistry)
                {
                    _reg_client->RegistryProvider(service->method(),_access_addr);
                }
                _router->registerMethod(service);
            }
            void Start()
            {
                _server->start();
            }
            private:
            bool _enableRegistry;
            Address _access_addr;
            client::RegistryClient::ptr _reg_client;
            RpcRouter::ptr _router;
            Dispatcher::ptr _dispatcher;
            BaseServer::ptr _server;

        };

        class TopicServer
        {
            public:
            using ptr=std::shared_ptr<TopicServer>;
            TopicServer(const int& port):_topic_manager(std::make_shared<TopicManager>()),
            _dispatcher(std::make_shared<Dispatcher>())
            {
                auto it1=std::bind(&TopicManager::Ontopicrequest,_topic_manager.get(),std::placeholders::_1,std::placeholders::_2);
                _dispatcher->registerHandler<TopicRequest>(MType::REQ_TOPIC,it1);

                _server=Serverfactory::create(port);
                auto it2=std::bind(&Dispatcher::OnMessage,_dispatcher.get(),std::placeholders::_1,std::placeholders::_2);
                _server->setMessageCallback(it2);

                auto it3=std::bind(&TopicServer::onConnectionShutdown,this,std::placeholders::_1);
                _server->setCloseCallback(it3);
            }
            void Start()
            {
                _server->start();
            }
            private:
            void onConnectionShutdown(const BaseConnection::ptr& conn)
            {
                _topic_manager->Onshutdown(conn);
            }
            private:
            TopicManager::ptr _topic_manager;
            Dispatcher::ptr _dispatcher;
            BaseServer::ptr _server;
        };
    }
}