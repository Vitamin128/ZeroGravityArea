#pragma once
#include"common/net.hpp"
#include"common/message.hpp"
#include"common/logger.hpp"
namespace gchrpc
{
    namespace server
    {
        enum class VType
        {
            BOOL=0,
            INTEGRAL,
            NUMERIC,
            STRING,
            ARRAY,
            OBJECT,
        };
        class ServiceDescribe
        {
            public:

            //指向自身的指针类型
            using ptr=std::shared_ptr<ServiceDescribe>;

            //服务的回调函数
            using ServiceCallBack=std::function<void(const Json::Value&,Json::Value&)>;

            //参数类型描述
            using ParamsDescribe=std::pair<std::string,VType>;

            //Success构造服务函数服务包括方法名,方法参数描述,返回值类型,回调函数
            ServiceDescribe(std::string&& mname,std::vector<ParamsDescribe>&& desc,
            VType vtype,ServiceCallBack&& handler):
            method_name(std::move(mname)),call_back(std::move(handler)),
            params_desc(std::move(desc)),return_type(vtype)
            {}

            //Success返回函数名
            const std::string& method()
            {
                return method_name;
            }

            //success校验Service的参数字段类型是否和Value类型是否相同
            bool paramCheck(const Json::Value& params)
            {
                for(auto& desc:params_desc)
                {
                    if(!params.isMember(desc.first))
                    {
                        LOG(ERROR) << "参数成员校验失败,存在缺失的参数:" << desc.first << std::endl;
                        return false;
                    }
                    if(!check(desc.second,params[desc.first]))
                    {
                        LOG(ERROR) << "参数校验失败,存在错误的类型参数:" << desc.first << std::endl;
                        return false;
                    }
                }
                return true;
            }

            //success用Service的回调函数处理参数并返回result
            bool call(const Json::Value& params,Json::Value& result)
            {
                call_back(params,result);
                if(!rtypeCheck(result))
                {
                    LOG(ERROR) << "返回参数类型校验失败" << std::endl;
                    return false;
                }
                return true;
            }
            private:

            //success校验返回值类型是否相同
            bool rtypeCheck(const Json::Value&val)
            {
                return check(return_type,val);
            }

            //success校验Json是否对应基准值
            bool check(VType vtype,const Json::Value& val)
            {
                switch (vtype)
                {
                    case VType::STRING: return val.isString();
                    case VType::ARRAY: return val.isArray();
                    case VType::BOOL: return val.isBool();
                    case VType::INTEGRAL: return val.isIntegral();
                    case VType::OBJECT: return val.isObject();
                    case VType::NUMERIC: return val.isNumeric();
                }
                return false;
            }
            std::string method_name; //服务函数名
            ServiceCallBack call_back; //服务函数
            std::vector<ParamsDescribe> params_desc; //函数的参数
            VType return_type; //函数的返回类型
        };


        class ServiceDescribeFactory
        {
            public:

            //Success
            void SetMethodName(const std::string& method_name)
            {
                _method_name=method_name;
            }

            //Success
            void SetServiceDescribe(const ServiceDescribe::ServiceCallBack& callback)
            {
                _callback=std::move(callback);    
            }

            //Success
            void SetParamsDesc(const std::string& param,VType vtype)
            {
                _params_desc.push_back({param,vtype});
            }

            //Success
            void SetReturnVtype(VType vtype)
            {
                _return_vtype=vtype;
            }

            //Success
            ServiceDescribe::ptr build()
            {
                return std::make_shared<ServiceDescribe>
                (std::move(_method_name),std::move(_params_desc)
                ,_return_vtype,std::move(_callback));
            }
            private:
            std::string _method_name;
            ServiceDescribe::ServiceCallBack _callback;
            std::vector<ServiceDescribe::ParamsDescribe>_params_desc;
            VType _return_vtype;
        };
        
        class ServiceManager
        {
            public:
            using ptr=std::shared_ptr<ServiceManager>;
            //success
            void insert(const ServiceDescribe::ptr& service)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _services[service->method()]=service;
            }
            //success
            ServiceDescribe::ptr select(const std::string& servicename)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                auto it=_services.find(servicename);
                if(it==_services.end())
                {
                    LOG(ERROR) << "服务未发现" << std::endl;
                    // return ServiceDescribe::ptr();
                    return nullptr;
                }
                return it->second;
            }
            //success
            void remove(const std::string& servicename)
            {
                std::unique_lock<std::mutex>lock(_mutex);
                _services.erase(servicename);
            }
            private:
            std::mutex _mutex;
            std::unordered_map<std::string,ServiceDescribe::ptr>_services;
        };


        class RpcRouter
        {
            public:
            using ptr=std::shared_ptr<RpcRouter>;

            //success
            RpcRouter():_service_manager(std::make_shared<ServiceManager>()){}

            //success服务端处理报文
            void onRpcRequest(const BaseConnection::ptr& conn,RpcRequest::ptr& request)
            {
                LOG(NORMAL) << "接收到 RPC 请求: " << request->method() << ", ID: " << request->rid() << std::endl;
                auto it=_service_manager->select(request->method());
                if(it==nullptr)
                {
                    LOG(ERROR) << "不存在对应请求的方法:" << request->method() << std::endl;
                    return response(conn,request,Json::Value(),Rcode::RCODE_NOT_FOUND_SERVICE);
                }

                bool ret=it->paramCheck(request->params());
                if(ret==false)
                {
                    LOG(ERROR) << "参数校验错误" << std::endl;
                    return response(conn,request,Json::Value(),Rcode::RCODE_INVALID_PARAMS);
                }

                Json::Value result;
                bool ret1=it->call(request->params(),result);
                if(ret1==false)
                {
                    LOG(ERROR) << "返回值错误" << std::endl;
                    return response(conn,request,Json::Value(),Rcode::RCODE_INTERNAL_ERROR);
                }

                return response(conn,request,result,Rcode::RCODE_OK);
            }

            //success注册一个服务的方法
            void registerMethod(const ServiceDescribe::ptr& service)
            {
                _service_manager->insert(service);
            }
            private:

            //success发送一个回复的报文
            void response(const BaseConnection::ptr& conn,const RpcRequest::ptr& rqs
                ,const Json::Value& body,Rcode rcode)
                {
                    RpcResponse::ptr rep=std::dynamic_pointer_cast
                    <RpcResponse>(MessageFactory::create(MType::RSP_RPC));
                    rep->setId(rqs->rid());
                    rep->setMType(MType::RSP_RPC);
                    rep->setRCode(rcode);
                    rep->setResult(body);
                    LOG(NORMAL) << "发送 RPC 响应, ID: " << rqs->rid() << ", 结果码: " << (int)rcode << std::endl;
                    conn->send(rep);
                }
            ServiceManager::ptr _service_manager;
        };
    }
}