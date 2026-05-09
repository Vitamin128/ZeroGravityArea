#pragma once
#include <functional>
#include <memory>

#include "client/rpc_client.hpp"
#include "common/dispatcher.hpp"
#include "common/logger.hpp"
#include "rpc_registry.hpp"
#include "rpc_router.hpp"
#include "rpc_topic.hpp"


namespace gchrpc {
namespace server {
class RegistryServer {
public:
    using ptr = std::shared_ptr<RegistryServer>;
    RegistryServer(const int &port)
        : _dispatcher(std::make_shared<Dispatcher>()), _pdmanager(std::make_shared<PDManager>()) {
        _server = Serverfactory::create(port);
        auto it1 = std::bind(&PDManager::onServiceResponse, _pdmanager.get(), std::placeholders::_1,
                             std::placeholders::_2);
        _dispatcher->registerHandler<ServiceRequest>(MType::REQ_SERVICE, it1);

        auto it2 = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1,
                             std::placeholders::_2);
        _server->setMessageCallback(it2);

        auto it3 = std::bind(&RegistryServer::ShutDown, this, std::placeholders::_1);
        _server->setCloseCallback(it3);
        LOG(NORMAL) << "RegistryServer 初始化完成，监听端口: " << port << std::endl;
    }
    void Start() {
        LOG(NORMAL) << "RegistryServer 启动..." << std::endl;
        _server->start();
    }
    void ShutDown(const BaseConnection::ptr conn) {
        LOG(NORMAL) << "RegistryServer 连接断开，清理服务提供者..." << std::endl;
        _pdmanager->onShutdown(conn);
    }

private:
    BaseServer::ptr _server;
    Dispatcher::ptr _dispatcher;
    PDManager::ptr _pdmanager;
};

class RpcServer {
public:
    using ptr = std::shared_ptr<RpcServer>;
    RpcServer(const Address &access_addr, bool enableRegistry = false,
              const Address &registry_server_addr = Address())
        : _enableRegistry(enableRegistry),
          _access_addr(access_addr),
          _router(std::make_shared<RpcRouter>()),
          _dispatcher(std::make_shared<Dispatcher>()) {
        if (_enableRegistry) {
            _reg_client = std::make_shared<client::RegistryClient>(registry_server_addr.first,
                                                                   registry_server_addr.second);
        }

        // _router=std::make_shared<RpcRouter>();

        auto it1 = std::bind(&RpcRouter::onRpcRequest, _router.get(), std::placeholders::_1,
                             std::placeholders::_2);

        _dispatcher->registerHandler<RpcRequest>(MType::REQ_RPC, it1);
        auto it2 = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1,
                             std::placeholders::_2);

        _server = Serverfactory::create(access_addr.second);
        _server->setMessageCallback(it2);
        LOG(NORMAL) << "RpcServer 初始化完成，访问地址: " << access_addr.first << ":"
                    << access_addr.second << std::endl;
    }
    void RegistryMethod(const ServiceDescribe::ptr &service) {
        if (_enableRegistry) {
            _reg_client->RegistryProvider(service->method(), _access_addr);
        }
        LOG(NORMAL) << "注册 RPC 方法: " << service->method() << std::endl;
        _router->registerMethod(service);
    }
    void Start() {
        LOG(NORMAL) << "RpcServer 启动..." << std::endl;
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

class TopicServer {
public:
    using ptr = std::shared_ptr<TopicServer>;
    TopicServer(const int &port)
        : _topic_manager(std::make_shared<TopicManager>()),
          _dispatcher(std::make_shared<Dispatcher>()) {
        auto it1 = std::bind(&TopicManager::Ontopicrequest, _topic_manager.get(),
                             std::placeholders::_1, std::placeholders::_2);
        _dispatcher->registerHandler<TopicRequest>(MType::REQ_TOPIC, it1);

        _server = Serverfactory::create(port);
        auto it2 = std::bind(&Dispatcher::OnMessage, _dispatcher.get(), std::placeholders::_1,
                             std::placeholders::_2);
        _server->setMessageCallback(it2);

        auto it3 = std::bind(&TopicServer::onConnectionShutdown, this, std::placeholders::_1);
        _server->setCloseCallback(it3);
        LOG(NORMAL) << "TopicServer 初始化完成，监听端口: " << port << std::endl;
    }
    void Start() {
        LOG(NORMAL) << "TopicServer 启动..." << std::endl;
        _server->start();
    }

private:
    void onConnectionShutdown(const BaseConnection::ptr &conn) {
        LOG(NORMAL) << "TopicServer 连接断开，清理订阅者信息..." << std::endl;
        _topic_manager->Onshutdown(conn);
    }

private:
    TopicManager::ptr _topic_manager;
    Dispatcher::ptr _dispatcher;
    BaseServer::ptr _server;
};
}  // namespace server
}  // namespace gchrpc