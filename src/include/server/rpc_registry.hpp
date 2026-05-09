#pragma once
#include "../common/net.hpp"
#include "../common/message.hpp"

#include <set>
/////
namespace gchrpc
{
    namespace server
    {
        class ProviderManager
        {
        public:
            using ptr = std::shared_ptr<ProviderManager>;

            //success一个提供者的connection和它所能提供的方法和IP
            struct Provider
            {
                using ptr = std::shared_ptr<Provider>;
                std::mutex _mutex;
                Address host;
                std::vector<std::string> methods;
                BaseConnection::ptr conn;
                Provider(const BaseConnection::ptr &NewConn, const Address &NewHost) : conn(NewConn), host(NewHost)
                {
                }
                void AppendMethod(const std::string &method)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    methods.push_back(method);
                }
            };

            //success选择提供者和方法,newhost是为了如果没有对应的提供者则添加
            void addprovider(const BaseConnection::ptr &newconn, const Address &newhost, const std::string &newmethod)
            {
                Provider::ptr newprovider;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    if (_conns.find(newconn) == _conns.end())
                    {
                        newprovider = std::make_shared<Provider>(newconn, newhost);
                        _conns[newconn] = newprovider;
                    }
                    else
                    {
                        newprovider = _conns[newconn];
                    }
                    std::set<Provider::ptr> &s1 = _providers[newmethod];
                    s1.insert(newprovider);
                }
                newprovider->AppendMethod(newmethod);
            }

            //success根据connection获取对应的提供者
            Provider::ptr GetProvider(const BaseConnection::ptr &conn)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = _conns.find(conn);
                    if (it != _conns.end())
                    {
                        return it->second;
                    }
                    return nullptr;
                }
            }

            //success删除对应的提供者
            void DelProvider(const BaseConnection::ptr &conn)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _conns.find(conn);
                if (it == _conns.end())
                {
                    return;
                }

                for (auto &item : it->second->methods)
                {
                    // _providers[item].erase(it->second);
                    auto it1=_providers.find(item);
                    if(it1!=_providers.end())
                    {
                        it1->second.erase(it->second);
                    }
                }
                _conns.erase(conn);
                return;
            }

            //success能提供方法的提供者的所有IP
            std::vector<Address> HostMethod(const std::string &method)
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _providers.find(method);
                if (it == _providers.end())
                {
                    return std::vector<Address>();
                }
                std::vector<Address> ret;
                for (auto &item : it->second)
                {
                    ret.push_back(item->host);
                }
                return ret;
            }

        private:
            std::mutex _mutex;
            std::unordered_map<std::string, std::set<Provider::ptr>> _providers;
            std::unordered_map<BaseConnection::ptr, Provider::ptr> _conns;
        };


        //success这里我们需要记住,就是客户端在它的内部会维护一个map表,这个表会有它的method对应的所有的服务端的host
        class DiscovererManager
        {
        public:
            using ptr = std::shared_ptr<DiscovererManager>;

            //success记录一个连接和多少个那些个方法对应
            struct Discoverer
            {
                using ptr = std::shared_ptr<Discoverer>;
                std::mutex _mutex;
                std::vector<std::string> methods;
                BaseConnection::ptr conn;
                Discoverer(const BaseConnection::ptr newconn) : conn(newconn) {}
                void AddMethod(const std::string &newmethod)
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    methods.push_back(newmethod);
                }
            };


            //success增加连接的方法
            Discoverer::ptr AddDiscoverer(const BaseConnection::ptr &conn, const std::string &newmethod)
            {
                Discoverer::ptr newdiscoverer;
                // std::unique_lock<std::mutex>lock(_mutex);
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = conns.find(conn);
                    if (it == conns.end())
                    {
                        newdiscoverer = std::make_shared<Discoverer>(conn);
                        conns[conn] = newdiscoverer;
                        ILOG("创建Discoverer客户端");
                    }
                    else
                    {
                        newdiscoverer = it->second;
                        ILOG("服务发现成功");
                    }

                    std::set<Discoverer::ptr> &set1 = discoverers[newmethod];
                    set1.insert(newdiscoverer);
                }
                newdiscoverer->AddMethod(newmethod);
                return newdiscoverer;
            }

            //success删除连接
            void DelDiscoverer(const BaseConnection::ptr &conn)
            {
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = conns.find(conn);
                    if (it == conns.end())
                    {
                        ILOG("不存在对应的连接");
                        return;
                    }
                    else
                    {
                        Discoverer::ptr discover = it->second;
                        auto &vec = discover->methods;
                        for (auto &meg : vec)
                        {
                            // discoverers[meg].erase(discover);
                            auto it=discoverers.find(meg);
                            if(it!=discoverers.end())
                            {
                                it->second.erase(discover);
                            }
                        }
                    }
                    conns.erase(conn);
                }
            }

            //success发送一个servicerequest给客户端，让客户端在维护的map添加一个方法对应的host

            void onlinenotify(const std::string &method, const Address &host)
            {
                notify(method,host,ServiceOptype::SERVICE_ONLINE);
            }

            //success发送一个servicerequest给客户端，让客户端在维护的map删除一个方法对应的host
            void offlinenotify(const std::string &method, const Address &host)
            {
                notify(method,host,ServiceOptype::SERVICE_OFFLINE);
            }
        private:

            //success给客户端发送一个servicerequest请求
            void notify(const std::string &method, const Address &host, const ServiceOptype &optype)
            {
                std::set<Discoverer::ptr> it1;
                {
                    std::unique_lock<std::mutex> lock(_mutex);
                    auto it = discoverers.find(method);
                    if (it == discoverers.end())
                    {
                        return;
                    }
                    it1 = it->second;
                }

                ServiceRequest::ptr msg_svr = MessageFactory::create<ServiceRequest>();
                msg_svr->setHost(host);
                msg_svr->setId(UUID::uuid());
                msg_svr->setMethod(method);
                msg_svr->setOptype(optype);

                for (auto &item : it1)
                {
                    item->conn->send(msg_svr);
                }
                return;
            }

            std::unordered_map<std::string, std::set<Discoverer::ptr>> discoverers;
            // std::unordered_map<std::string,std::set<BaseConnection::ptr>> discoverers;
            // std::unordered_map<std::string,std::vector<Discoverer>> discoverers;
            std::unordered_map<BaseConnection::ptr, Discoverer::ptr> conns;
            std::mutex _mutex;
        };

        class PDManager
        {
            public:
            using ptr=std::shared_ptr<PDManager>;
            PDManager():_providers(std::make_shared<ProviderManager>()),_discovers(std::make_shared<DiscovererManager>())
            {}

            //success注册的对外接口用于处理连接和连接发送的请求
            void onServiceResponse(const BaseConnection::ptr& conn,const ServiceRequest::ptr& svr_msg)
            {
                ServiceOptype svopt=svr_msg->optype();
                if(svopt==ServiceOptype::SERVICE_DISCOVERY)
                {
                    
                    ILOG("%s服务发现:%s",svr_msg->rid().c_str(),svr_msg->method().c_str());
                    _discovers->AddDiscoverer(conn,svr_msg->method());
                    return Discoverresponse(conn,svr_msg);
                }
                else if(svopt==ServiceOptype::SERVICE_REGISTRY)
                {
                    ILOG("%s:%d,服务注册:%s",svr_msg->host().first.c_str(),svr_msg->host().second,svr_msg->method().c_str());
                    _providers->addprovider(conn,svr_msg->host(),svr_msg->method());
                    _discovers->onlinenotify(svr_msg->method(),svr_msg->host());
                    return Registryresponse(conn,svr_msg);
                }
                else
                {
                    ILOG("服务类型错误");
                    return Errorresponse(conn,svr_msg);
                }
            }

            //success关闭对应的连接和连接对应的服务
            void onShutdown(const BaseConnection::ptr& conn)
            {
                auto it=_providers->GetProvider(conn);
                if(it.get()!=nullptr)
                {
                    for(auto item:it->methods)
                    {
                        _discovers->offlinenotify(item,it->host);
                    }
                    _providers->DelProvider(conn);
                }
                _discovers->DelDiscoverer(conn);
            }
            private:

            //success服务错误回复
            void Errorresponse(const BaseConnection::ptr& conn,const ServiceRequest::ptr& sep)
            {
                ServiceResponse::ptr msg_rep=MessageFactory::create<ServiceResponse>();
                msg_rep->setId(sep->rid());
                msg_rep->setRCode(Rcode::RCODE_INVALID_OPTYPE);
                msg_rep->setoptype(ServiceOptype::SERVICE_UNKNOW);
                conn->send(msg_rep);
            }


            //success服务注册回复serviceresponse
            void Registryresponse(const BaseConnection::ptr& conn,const ServiceRequest::ptr& sep)
            {
                ServiceResponse::ptr msg_rep=MessageFactory::create<ServiceResponse>();
                msg_rep->setId(sep->rid());
                msg_rep->setRCode(Rcode::RCODE_OK);
                msg_rep->setoptype(ServiceOptype::SERVICE_REGISTRY);
                conn->send(msg_rep);
            }


            //success服务发现回复serviceresponse
            void Discoverresponse(const BaseConnection::ptr& conn,const ServiceRequest::ptr& sep)
            {
                ServiceResponse::ptr msg_rep=MessageFactory::create<ServiceResponse>();
                msg_rep->setId(sep->rid());
                msg_rep->setoptype(ServiceOptype::SERVICE_DISCOVERY);
                std::vector<Address> hosts=_providers->HostMethod(sep->method());
                if(hosts.empty())
                {
                    msg_rep->setRCode(Rcode::RCODE_NOT_FOUND_SERVICE);
                    conn->send(msg_rep);
                    return;
                }
                msg_rep->setHost(hosts);
                msg_rep->setRCode(Rcode::RCODE_OK);
                msg_rep->setMethod(sep->method());
                conn->send(msg_rep);
                
            }

            ProviderManager::ptr _providers;
            DiscovererManager::ptr _discovers;
        };
    }
}