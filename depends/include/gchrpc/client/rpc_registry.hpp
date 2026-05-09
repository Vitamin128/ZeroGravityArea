#pragma once
#include "requestor.hpp"
#include <unordered_set>

namespace gchrpc
{
    namespace client
    {
        class Provider
        {
        public:
            using ptr = std::shared_ptr<Provider>;
            Provider(const Requestor::ptr &requestor) : _requestor(requestor) {}

            // 向注册中心注册方法（携带自身地址）
            bool registrymethod(const BaseConnection::ptr &conn, const std::string &method, const Address &host)
            {
                ServiceRequest::ptr seq = MessageFactory::create<ServiceRequest>();
                seq->setHost(host);
                seq->setMethod(method);
                seq->setId(UUID::uuid());
                seq->setOptype(ServiceOptype::SERVICE_REGISTRY);
                LOG(NORMAL) << "发送服务注册请求, method=" << method << ", host=" << host.first << ":" << host.second << std::endl;
                BaseMessage::ptr rep;
                bool ret = _requestor->send(conn, seq, rep);
                if (ret == false)
                {
                    LOG(ERROR) << "服务注册请求发送失败, method=" << method << std::endl;
                    return false;
                }
                ServiceResponse::ptr sep = std::dynamic_pointer_cast<ServiceResponse>(rep);
                if (sep.get() == nullptr)
                {
                    LOG(ERROR) << "服务注册响应类型转换失败, method=" << method << std::endl;
                    return false;
                }
                if (sep->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "服务注册失败: " << errReason(sep->rcode()) << std::endl;
                    return false;
                }
                LOG(NORMAL) << "服务注册成功, method=" << method << std::endl;
                return true;
            }

        private:
            Requestor::ptr _requestor;
        };

        class MethodHost
        {
        public:
            using ptr = std::shared_ptr<MethodHost>;
            MethodHost() : _idx(0) {}
            MethodHost(const std::vector<Address> &hosts) : _idx(0), _hosts(hosts.begin(), hosts.end()) {}

            void AddMethodHost(const Address &host)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _hosts.push_back(host);
                LOG(NORMAL) << "新增服务地址: " << host.first << ":" << host.second << std::endl;
            }

            void RemoveHost(const Address &host)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                for (auto it = _hosts.begin(); it != _hosts.end(); it++)
                {
                    if (it->second == host.second && it->first == host.first)
                    {
                        _hosts.erase(it);
                        LOG(NORMAL) << "移除服务地址: " << host.first << ":" << host.second << std::endl;
                        break;
                    }
                }
            }

            Address GetHost()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                if (!_hosts.empty())
                {
                    size_t index = (_idx++) % _hosts.size();
                    return _hosts[index];
                }
                return Address();
            }

            bool empty()
            {
                std::unique_lock<std::mutex> lock(_mutex);
                return _hosts.empty();
            }

        private:
            std::mutex _mutex;
            size_t _idx;
            std::vector<Address> _hosts;
        };

        class Discoverer
        {
        public:
            using ptr = std::shared_ptr<Discoverer>;
            using offlinefunction = std::function<void(const Address &)>;
            Discoverer(const offlinefunction &offfunction, const Requestor::ptr &requestor)
                : _off_function(offfunction), _requestor(requestor) {}

            // 查找对应 method 的服务地址，若本地无缓存则向注册中心发起服务发现请求
            bool ServiceDiscover(const BaseConnection::ptr &conn, const std::string &method, Address &host)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = MethodHosts.find(method);
                    if (it != MethodHosts.end())
                    {
                        if (it->second && !it->second->empty())
                        {
                            host = it->second->GetHost();
                            LOG(NORMAL) << "命中本地服务缓存, method=" << method << ", host=" << host.first << ":" << host.second << std::endl;
                            return true;
                        }
                    }
                }
                LOG(NORMAL) << "本地无缓存，向注册中心发起服务发现, method=" << method << std::endl;
                ServiceRequest::ptr req = MessageFactory::create<ServiceRequest>();
                req->setId(UUID::uuid());
                req->setMethod(method);
                req->setOptype(ServiceOptype::SERVICE_DISCOVERY);
                BaseMessage::ptr rep;
                bool ret = _requestor->send(conn, req, rep);
                if (ret == false)
                {
                    LOG(ERROR) << "服务发现请求发送失败, method=" << method << std::endl;
                    return false;
                }
                ServiceResponse::ptr msg = std::dynamic_pointer_cast<ServiceResponse>(rep);
                if (msg == nullptr)
                {
                    LOG(ERROR) << "服务发现响应类型转换失败, method=" << method << std::endl;
                    return false;
                }
                if (msg->rcode() != Rcode::RCODE_OK)
                {
                    LOG(ERROR) << "服务发现失败: " << errReason(msg->rcode()) << std::endl;
                    return false;
                }
                std::vector<Address> hosts = msg->hosts();
                MethodHost::ptr newhosts = std::make_shared<MethodHost>(hosts);
                if (newhosts->empty())
                {
                    LOG(ERROR) << "服务发现成功但无可用地址, method=" << method << std::endl;
                    return false;
                }
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    host = newhosts->GetHost();
                    MethodHosts[method] = newhosts;
                    LOG(NORMAL) << "服务发现完成, method=" << method << ", host=" << host.first << ":" << host.second << std::endl;
                    return true;
                }
            }

            void onServiceRequest(const BaseConnection::ptr &conn, const ServiceRequest::ptr &msg)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (msg->optype() == ServiceOptype::SERVICE_ONLINE)
                    {
                        LOG(NORMAL) << "收到服务上线通知, method=" << msg->method() << std::endl;
                        auto it = MethodHosts.find(msg->method());
                        if (it != MethodHosts.end())
                        {
                            it->second->AddMethodHost(msg->host());
                        }
                        else
                        {
                            MethodHost::ptr newMethodHost = std::make_shared<MethodHost>();
                            newMethodHost->AddMethodHost(msg->host());
                            MethodHosts[msg->method()] = newMethodHost;
                        }
                        return;
                    }
                    else if (msg->optype() == ServiceOptype::SERVICE_OFFLINE)
                    {
                        LOG(NORMAL) << "收到服务下线通知, method=" << msg->method() << std::endl;
                        auto it = MethodHosts.find(msg->method());
                        if (it != MethodHosts.end())
                        {
                            it->second->RemoveHost(msg->host());
                        }
                        else
                        {
                            LOG(ERROR) << "收到下线通知但本地无该服务记录, method=" << msg->method() << std::endl;
                            return;
                        }
                    }
                    else
                    {
                        LOG(ERROR) << "服务操作类型错误, optype=" << (int)msg->optype() << std::endl;
                        return;
                    }
                }
                _off_function(msg->host());
            }

        private:
            std::mutex _mutex;
            Requestor::ptr _requestor;
            offlinefunction _off_function;
            std::unordered_map<std::string, MethodHost::ptr> MethodHosts;
        };
    }
}