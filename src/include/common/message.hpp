#pragma once
#include"logger.hpp"
#include"detail.hpp"
#include"fields.hpp"
#include"abstract.hpp"

namespace gchrpc
{
    
    typedef std::pair<std::string,int> Address;


    //用于存储Json格式的Message,也可以将保留的Json格式的对象序列化返回
    class JsonMessage:public BaseMessage{
        public:
        using ptr=std::shared_ptr<JsonMessage>;

        virtual std::string serialize() override{
            std::string body;
            bool ret=JSON::serialize(_body,body);
            if(ret==false)
            {
                return std::string();
            }
            return body;
        }

        virtual bool unserialize(std::string& msg)override
        {
            return JSON::unserialize(msg,_body);
        }
        protected:
        Json::Value _body;
    };


    //Json格式的请求,只有Json::Value _body和MType _mtype和std::string _rid;
    class JsonRequest:public JsonMessage{
        public:
        using ptr=std::shared_ptr<JsonRequest>;
    };


    //返回的消息,返回的消息和请求消息的区别是返回的消息有返回状态码,而请求消息只有请求码
    class JsonResponse:public JsonMessage{
        public:
        using ptr=std::shared_ptr<JsonResponse>;

        virtual bool check()override{
            if(_body[KEY_RCODE].isNull()==true)
            {
                LOG(ERROR) << "响应中无状态码" << std::endl;
                return false;
            }
            if(_body[KEY_RCODE].isIntegral()==false)
            {
                LOG(ERROR) << "响应状态码类型错误" << std::endl;
            }
            return true;
        }

        virtual Rcode rcode()
        {
            return (Rcode)_body[KEY_RCODE].asInt();
        }

        virtual void setRCode(Rcode rcode){
            _body[KEY_RCODE]=(int)rcode;
        }
    };


    //Rpc设置请求Message的参数,而请求系统的方法
    class RpcRequest:public JsonRequest{
        public:
        using ptr=std::shared_ptr<RpcRequest>;

        RpcRequest(){setMType(MType::REQ_RPC);}

        virtual bool check()override{
            if(_body[KEY_METHOD].isNull()==true||
            _body[KEY_METHOD].isString()==false)
            {
                LOG(ERROR) << "RPC请求中没有方法名称或方法名称类型错误" << std::endl;
                return false;
            }
            if(_body[KEY_PARAMS].isNull()==true||
            _body[KEY_PARAMS].isObject()==false)
            {
                LOG(ERROR) << "RPC请求中没有参数或参数类型错误" << std::endl;
                return false;
            }
            return true;
        }

        std::string method()
        {
            return _body[KEY_METHOD].asString();
        }

        void setMethod(const std::string &method_name)
        {
            _body[KEY_METHOD]=method_name;
        }


        Json::Value params()
        {
            return _body[KEY_PARAMS];
        }

        void setParams(const Json::Value& params)
        {
            _body[KEY_PARAMS]=params;
        }
    };


    class TopicRequest:public JsonRequest{
        public:
        using ptr=std::shared_ptr<TopicRequest>;

        TopicRequest(){setMType(MType::REQ_TOPIC);}

        virtual bool check()override{
            if(_body[KEY_TOPIC_KEY].isNull()==true||
                _body[KEY_TOPIC_KEY].isString()==false)
            {
                LOG(ERROR) << "主题请求中没有主题名称或主题名称类型错误!" << std::endl;
                return false;
            }
            if(_body[KEY_OPTYPE].isNull()==true||
                _body[KEY_OPTYPE].isIntegral()==false)
            {
                LOG(ERROR) << "主题请求中没有操作类型或操作类型名称类型错误" << std::endl;
                return false;
            }
            if(_body[KEY_OPTYPE].asInt()==(int)TopicOptype::TOPIC_PUBLISH && 
            (_body[KEY_TOPIC_MSG].isNull()==true ||
             _body[KEY_TOPIC_MSG].isString()==false))
            {
                LOG(ERROR) << "主题消息发布请求中没有消息内容字段或消息内容类型错误" << std::endl;
                return false;
            }
            return true;
        }

        std::string topicKey()
        {
            return _body[KEY_TOPIC_KEY].asString();
        }


        void setTopicKey(const std::string& key)
        {
            _body[KEY_TOPIC_KEY]=key;
        }

        TopicOptype optype()
        {
            return (TopicOptype)_body[KEY_OPTYPE].asInt();
        }

        void setOptype(TopicOptype optype)
        {
            _body[KEY_OPTYPE]=(int)optype;
        }

        std::string topicMsg()
        {
            return _body[KEY_TOPIC_MSG].asString();
        }

        void setTopicMsg(const std::string& msg)
        {
            _body[KEY_TOPIC_MSG]=msg;
        }
    };


    class ServiceRequest:public JsonRequest{
        public:

        ServiceRequest(){setMType(MType::REQ_SERVICE);}
        using ptr=std::shared_ptr<ServiceRequest>;


        virtual bool check() override{
            if(_body[KEY_METHOD].isNull()==true||
            _body[KEY_METHOD].isString()==false)
            {
                LOG(ERROR) << "服务请求中没有方法名称或方法名称类型错误!" << std::endl;
                return false;
            }
            if(_body[KEY_OPTYPE].isNull()==true||
            _body[KEY_OPTYPE].isIntegral()==false)
            {
                LOG(ERROR) << "服务请求中没有操作类型或操作类型的类型错误!" << std::endl;
                return false;
            }
            if(_body[KEY_OPTYPE].asInt()!=(int)(ServiceOptype::SERVICE_DISCOVERY)&&
            (_body[KEY_HOST].isNull()==true||
            _body[KEY_HOST].isObject()==false||
            _body[KEY_HOST][KEY_HOST_IP].isNull()==true||
            _body[KEY_HOST][KEY_HOST_IP].isString()==false||
            _body[KEY_HOST][KEY_HOST_PORT].isNull()==true||
            _body[KEY_HOST][KEY_HOST_PORT].isIntegral()==false))
            {
                LOG(ERROR) << "服务请求中主题地址错误!" << std::endl;
                return false;
            }
            return true;
        }

        std::string method()
        {
            return _body[KEY_METHOD].asString();
        }

        void setMethod(const std::string& name)
        {
            _body[KEY_METHOD]=name;
        }

        ServiceOptype optype()
        {
            return (ServiceOptype)_body[KEY_OPTYPE].asInt();
        }

        void setOptype(ServiceOptype optype)
        {
            _body[KEY_OPTYPE]=(int)(optype);
        }

        Address host()
        {
            Address addr;
            addr.first=_body[KEY_HOST][KEY_HOST_IP].asString();
            addr.second=_body[KEY_HOST][KEY_HOST_PORT].asInt();
            return addr;
        }

        void setHost(const Address& addr)
        {
            _body[KEY_HOST][KEY_HOST_IP]=addr.first;
            _body[KEY_HOST][KEY_HOST_PORT]=addr.second;

        }
    };

    class RpcResponse:public JsonResponse
    {
        public:

        RpcResponse(){setMType(MType::RSP_RPC);}
        using ptr=std::shared_ptr<RpcResponse>;

        virtual bool check()override{
            if(_body[KEY_RCODE].isNull()==true||
                _body[KEY_RCODE].isInt()==false)
            {
                LOG(ERROR) << "响应回复状态码为空,或状态码类型错误" << std::endl;
                return false;
            }
            if(_body[KEY_RESULT].isNull()==true)
            {
                LOG(ERROR) << "响应结果为空,或响应结果类型错误" << std::endl;
                return false;
            }
            return true;
        }

        Json::Value result()
        {
            return _body[KEY_RESULT];
        }

        void setResult(const Json::Value& result)
        {
            _body[KEY_RESULT]=result;
        }
    };


    class TopicResponse:public JsonResponse
    {
        public:
        using ptr=std::shared_ptr<TopicResponse>;
    };

    class HeartbeatRequest: public JsonRequest
    {
        public:
        using ptr=std::shared_ptr<HeartbeatRequest>;
        HeartbeatRequest(){setMType(MType::REQ_HEARTBEAT);}
        virtual bool check() override { return true; }
    };

    class HeartbeatResponse: public JsonResponse
    {
        public:
        using ptr=std::shared_ptr<HeartbeatResponse>;
        HeartbeatResponse(){setMType(MType::RSP_HEARTBEAT);}
        virtual bool check() override { return true; }
    };
    
    class LoadInfoRequest: public JsonRequest
    {
        public:
        using ptr=std::shared_ptr<LoadInfoRequest>;
        LoadInfoRequest(){setMType(MType::REQ_LOAD_INFO);}
        virtual bool check() override { return true; }
        void setLoad(double val) { _body[KEY_LOAD_VAL] = val; }
        double load() { return _body[KEY_LOAD_VAL].asDouble(); }
    };

    class ServiceResponse:public JsonResponse
    {
        public:

        ServiceResponse(){setMType(MType::RSP_SERVICE);}
        using ptr=std::shared_ptr<ServiceResponse>;

        virtual bool check()override{
            if(_body[KEY_RCODE].isNull()==true||
            _body[KEY_RCODE].isIntegral()==false)
            {
                LOG(ERROR) << "响应中没有状态码或者状态码类型错误" << std::endl;
                return false;
            }
            if(_body[KEY_OPTYPE].isNull()==true||
            _body[KEY_OPTYPE].isIntegral()==false)
            {
                LOG(ERROR) << "响应中没有操作类型或者操作类型的类型错误" << std::endl;
                return false;
            }

            if((_body[KEY_OPTYPE]==(int)ServiceOptype::SERVICE_DISCOVERY)&&
            (_body[KEY_METHOD].isNull()==true||
            _body[KEY_METHOD].isString()==false||
            _body[KEY_HOST].isNull()==true||
            _body[KEY_HOST].isArray()==false))
            {
                LOG(ERROR) << "服务发现响应中信息字段错误" << std::endl;
                return false;
            }
            return true;
        }

        ServiceOptype optype()
        {
            return (ServiceOptype)_body[KEY_OPTYPE].asInt();
        }

        void setoptype(const ServiceOptype& optype)
        {
            _body[KEY_OPTYPE]=(int)optype;
        }

        std::string method()
        {
            return _body[KEY_METHOD].asString();
        }

        void setMethod(const std::string& method)
        {
            _body[KEY_METHOD]=method;
        }

        void setHost(std::vector<Address>addrs)
        {
            for(auto& addr:addrs)
            {
                Json::Value newValue;
                newValue[KEY_HOST_IP]=addr.first;
                newValue[KEY_HOST_PORT]=addr.second;
                _body[KEY_HOST].append(newValue);
            }
        }

        std::vector<Address> hosts()
        {
            // std::vector<Address> address(0);
            int sz=_body[KEY_HOST].size();
            std::vector<Address> address(sz);
            for(int i=0;i<sz;i++)
            {
                address[i].first=_body[KEY_HOST][i][KEY_HOST_IP].asString();
                address[i].second=_body[KEY_HOST][i][KEY_HOST_PORT].asInt();
            }
            return address;
        }
    };

    class MessageFactory{
        public:

        //返回基类
        static BaseMessage::ptr create(MType mtype)
        {
            switch (mtype)
            {
                case MType::REQ_RPC:
                return std::make_shared<RpcRequest>();
                case MType::RSP_RPC:
                return std::make_shared<RpcResponse>();
                case MType::REQ_TOPIC:
                return std::make_shared<TopicRequest>();
                case MType::RSP_TOPIC:
                return std::make_shared<TopicResponse>();
                case MType::REQ_SERVICE:
                return std::make_shared<ServiceRequest>();
                case MType::RSP_SERVICE:
                return std::make_shared<ServiceResponse>();
                case MType::REQ_HEARTBEAT:
                return std::make_shared<HeartbeatRequest>();
                case MType::RSP_HEARTBEAT:
                return std::make_shared<HeartbeatResponse>();
                case MType::REQ_LOAD_INFO:
                return std::make_shared<LoadInfoRequest>();
            }
            return BaseMessage::ptr();
        }

        //返回派生类
        template<typename T,typename ...Args>
        static std::shared_ptr<T> create(Args&& ...args)
        {
            return std::make_shared<T>(std::forward<Args>(args)...);
        }
    };
}