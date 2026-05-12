#pragma once
#include <string>
#include <unordered_map>
namespace gchrpc
{

//_body中的key值
#define KEY_METHOD "method"
#define KEY_PARAMS "parameters"
#define KEY_TOPIC_KEY "topic_key"
#define KEY_TOPIC_MSG "topic_msg"
#define KEY_OPTYPE "optype"
#define KEY_HOST "host"
#define KEY_HOST_IP "ip"
#define KEY_HOST_PORT "port"
#define KEY_RCODE "rcode"
#define KEY_RESULT "result"
#define KEY_LOAD_VAL "load_val"

    //message类型分别为RPC请求和回复,TOPIC请求和回复,SERVICE请求和回复
    enum class MType
    {
        REQ_RPC = 0,
        RSP_RPC,
        REQ_TOPIC,
        RSP_TOPIC,
        REQ_SERVICE,
        RSP_SERVICE,
        REQ_HEARTBEAT,
        RSP_HEARTBEAT
    };

    //message的回复码,用于回复消息的结果定义
    enum class Rcode
    {
        RCODE_OK = 0,
        RCODE_PARSE_FAILED,
        RCODE_ERROR_MSGTYPE,
        RCODE_INVALID_MSG,
        RCODE_DISCONNECTED,
        RCODE_INVALID_PARAMS,
        RCODE_NOT_FOUND_SERVICE,
        RCODE_INVALID_OPTYPE,
        RCODE_NOT_FOUND_TOPIC,
        RCODE_INTERNAL_ERROR
    };

    //根据回复码返回String类型的信息
    static std::string errReason(Rcode code)
    {
        static std::unordered_map<Rcode, std::string> err_map = {
            {Rcode::RCODE_OK, "成功处理"},
            {Rcode::RCODE_PARSE_FAILED, "消息解析失败"},
            {Rcode::RCODE_ERROR_MSGTYPE, "消息类型错误"},
            {Rcode::RCODE_INVALID_MSG, "无效消息"},
            {Rcode::RCODE_DISCONNECTED, "连接已断开!"},
            {Rcode::RCODE_INVALID_PARAMS, "无效的RPC参数!"},
            {Rcode::RCODE_NOT_FOUND_SERVICE, "未发现对应服务"},
            {Rcode::RCODE_INVALID_OPTYPE, "无效操作类型"},
            {Rcode::RCODE_NOT_FOUND_TOPIC, "没有找到对应的订阅"},
            {Rcode::RCODE_INTERNAL_ERROR, "内部错误"},
        };
        auto it = err_map.find(code);
        if (it == err_map.end())
        {
            return "未知错误";
        }
        return it->second;
    }


    enum class RType
    {
        REQ_ASYNC=0,
        REQ_CALLBACK
    };


    //主题操作
    enum class TopicOptype{
        TOPIC_CREATE=0,
        TOPIC_REMOVE,
        TOPIC_SUBSCRIBE,
        TOPIC_CANCEL,
        TOPIC_PUBLISH
    };

    enum class ServiceOptype
    {
        SERVICE_REGISTRY=0,//服务注册
        SERVICE_DISCOVERY,//服务发现
        SERVICE_ONLINE,//服务上线
        SERVICE_OFFLINE,//服务下线
        SERVICE_UNKNOW//位置服务
    };
}
