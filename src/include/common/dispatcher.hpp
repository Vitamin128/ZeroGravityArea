#pragma once
#include"net.hpp"
#include"message.hpp"

namespace gchrpc
{
    class Callback{
        public:
        using ptr=std::shared_ptr<Callback>;
        virtual void onMessage(const BaseConnection::ptr& conn,BaseMessage::ptr& msg)=0;
    };

    //Success这里的_handler
    template<typename T>
    class CallbackT:public Callback
    {
        public:
        using ptr=std::shared_ptr<CallbackT<T>>;
        using MessageCallBack=std::function<void(const BaseConnection::ptr& conn,std::shared_ptr<T>& msg)>;
        
        //Success
        CallbackT(const MessageCallBack& handler):_handler(handler){}

        //Success
        void onMessage(const BaseConnection::ptr& conn,BaseMessage::ptr& msg)
        {
            auto it=std::dynamic_pointer_cast<T>(msg);
            _handler(conn,it);
        }
        private:
        MessageCallBack _handler;
    };

    class Dispatcher
    {
        public:
        using ptr=std::shared_ptr<Dispatcher>;

        //Success针对于每个消息类型进行自定义回调函数的处理;
        template<typename T>//success ////const std::function<void(const BaseConnection::ptr& conn,std::shared_ptr<T>& msg)>
        void registerHandler(MType mtype,const typename CallbackT<T>::MessageCallBack& handler)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto item=std::make_shared<CallbackT<T>>(handler);
            _handlers.insert(std::make_pair(mtype,item));
        }

        //Success使用CallbackT内置的函数处理相关的connection和basemessage
        void OnMessage(const BaseConnection::ptr& conn,BaseMessage::ptr& msg)
        {
            std::unique_lock<std::mutex> lock(_mutex);
            auto it=_handlers.find(msg->mtype());
            if(it!=_handlers.end())
            {
                return it->second->onMessage(conn,msg);
            }
            LOG(ERROR) << "未找到对应的消息处理器, mtype=" << (int)msg->mtype() << std::endl;
            conn->shutdown();
        }
        private:
        std::mutex _mutex;
        std::unordered_map<MType,Callback::ptr> _handlers;
    };
}