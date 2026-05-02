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

            // success注册方法,客户端也是可以带着地址去注册方法的
            bool registrymethod(const BaseConnection::ptr &conn, const std::string &method, const Address &host)
            {
                ServiceRequest::ptr seq = MessageFactory::create<ServiceRequest>();
                seq->setHost(host);
                seq->setMethod(method);
                seq->setId(UUID::uuid());
                seq->setOptype(ServiceOptype::SERVICE_REGISTRY);
                BaseMessage::ptr rep;
                bool ret = _requestor->send(conn, seq, rep);
                if (ret == false)
                {
                    ILOG("发送失败%s:%s", seq->method().c_str(), seq->rid().c_str());
                    return false;
                }
                ServiceResponse::ptr sep = std::dynamic_pointer_cast<ServiceResponse>(rep);
                if (sep.get() == nullptr)
                {
                    ILOG("向下转换类型失败");
                    return false;
                }
                if (sep->rcode() != Rcode::RCODE_OK)
                {
                    ILOG("请求处理失败:%s", errReason(sep->rcode()).c_str());
                    return false;
                }
                ILOG("发送请求处理成功");
                return true;
            }

        private:
            Requestor::ptr _requestor;
        };

        class MethodHost
        {
        public:
            using ptr = std::shared_ptr<MethodHost>;
            MethodHost() : _idx(0)
            {
            }
            MethodHost(const std::vector<Address> &hosts) : _idx(0), _hosts(hosts.begin(), hosts.end())
            {
            }
            void AddMethodHost(const Address &host)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _hosts.push_back(host);
            }
            void RemoveHost(const Address &host)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                for (auto it = _hosts.begin(); it != _hosts.end(); it++)
                {
                    if (it->second == host.second && it->first == host.first)
                    {
                        _hosts.erase(it);
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
                // return _hosts[index];
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
            Discoverer(const offlinefunction &offfunction, const Requestor::ptr &requestor) : _off_function(offfunction), _requestor(requestor)
            {}


            // success查找对应的method服务,如果没有该服务则发送一个服务请求,收到服务回复则构建一个服务
            bool ServiceDiscover(const BaseConnection::ptr &conn, const std::string &method, Address &host)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = MethodHosts.find(method);
                    if (it!= MethodHosts.end())
                    {
                        // auto it = MethodHosts.find(method);
                        if (it->second && !it->second->empty())
                        {
                            host = it->second->GetHost();
                            return true;
                        }
                    }
                }
                ServiceRequest::ptr req = MessageFactory::create<ServiceRequest>();
                req->setId(UUID::uuid());
                req->setMethod(method);
                req->setOptype(ServiceOptype::SERVICE_DISCOVERY);
                BaseMessage::ptr rep;
                bool ret = _requestor->send(conn, req, rep);

                if (ret == false)
                {
                    ELOG("服务发现失败");
                    return false;
                }
                ServiceResponse::ptr msg = std::dynamic_pointer_cast<ServiceResponse>(rep);
                if (msg == nullptr)
                {
                    ILOG("向下类型转化失败");
                    return false;
                }
                if (msg->rcode() != Rcode::RCODE_OK)
                {
                    ELOG("消息返回码错误:%s", errReason(msg->rcode()).c_str());
                    return false;
                }
                // auto it=MethodHosts.find(method);
                std::vector<Address> hosts = msg->hosts();
                MethodHost::ptr newhosts = std::make_shared<MethodHost>(hosts);
                if (newhosts->empty())
                {
                    ELOG("无该服务的提供");
                    return false;
                }

                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    host = newhosts->GetHost();
                    MethodHosts[method] = newhosts;
                    return true;
                }
            }

            void onServiceRequest(const BaseConnection::ptr &conn, const ServiceRequest::ptr &msg)
            {
                {
                    std::unique_lock<std::mutex>lock(_mutex);
                    if(msg->optype()==ServiceOptype::SERVICE_ONLINE)
                    {
                        auto it=MethodHosts.find(msg->method());
                        if(it!=MethodHosts.end())
                        {
                            it->second->AddMethodHost(msg->host());
                        }
                        else
                        {
                            MethodHost::ptr newMethodHost=std::make_shared<MethodHost>();
                            newMethodHost->AddMethodHost(msg->host());
                            MethodHosts[msg->method()]=newMethodHost;
                        }
                        return;

                    }
                    else if(msg->optype()==ServiceOptype::SERVICE_OFFLINE)
                    {
                        auto it=MethodHosts.find(msg->method());
                        if(it!=MethodHosts.end())
                        {
                            it->second->RemoveHost(msg->host());
                        }
                        else
                        {
                            ELOG("不存在该服务");
                            return;
                        }
                    }
                    else
                    {
                        ELOG("服务操作类型错误");
                        return;
                    }
                }
                _off_function(msg->host());
            }

        private:
            std::mutex _mutex;

            // success消息转接区
            Requestor::ptr _requestor;
            offlinefunction _off_function;

            // success一个服务对应的几个服务器地址
            std::unordered_map<std::string, MethodHost::ptr> MethodHosts;
        };
    }
}