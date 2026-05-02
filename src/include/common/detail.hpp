#pragma once
#include "logger.hpp"
#include <jsoncpp/json/json.h>

#include <atomic>
#include <iomanip>
#include <random>
#include <sstream>

// 前向声明，detail.hpp 被 util.hpp 包含之前 LOG 宏已通过 util.hpp 定义
// 因此这里直接使用 LOG 宏即可

namespace gchrpc {

// JSON类,定义了序列化和反序列化方法
class JSON {
public:
    // Json::Value----->String
    static bool serialize(const Json::Value &val, std::string &body) {
        std::stringstream ss;
        Json::StreamWriterBuilder swb;
        swb["emitUTF8"] = true;
        std::unique_ptr<Json::StreamWriter> sw(swb.newStreamWriter());
        int ret = sw->write(val, &ss);
        if (ret != 0) {
            LOG(ERROR) << "JSON 序列化失败" << std::endl;
            return false;
        }
        body = ss.str();
        return true;
    }

    // String----->Json::value
    static bool unserialize(const std::string &body, Json::Value &val) {
        Json::CharReaderBuilder crb;
        std::string errs;
        std::unique_ptr<Json::CharReader> cr(crb.newCharReader());
        bool ret = cr->parse(body.c_str(), body.c_str() + body.size(), &val, &errs);
        if (ret == false) {
            LOG(ERROR) << "JSON 反序列化失败: " << errs << std::endl;
            return false;
        }
        return true;
    }
};

//! 生成一个密钥比如:ad2af789-d53c-154f-0000-000000000001,也不想看,看不懂
class UUID {
public:
    static std::string uuid() {
        std::stringstream ss;

        std::random_device rd;

        std::mt19937 generator(rd());

        std::uniform_int_distribution<int> distribution(0, 255);
        for (int i = 0; i < 8; i++) {
            if (i == 4 || i == 6) {
                ss << "-";
            }
            ss << std::setw(2) << std::setfill('0') << std::hex << distribution(generator);
        }
        ss << "-";
        static std::atomic<size_t> seq(1);
        size_t cur = seq.fetch_add(1);
        for (int i = 7; i >= 0; i--) {
            if (i == 5) {
                ss << "-";
            }
            ss << std::setw(2) << std::setfill('0') << std::hex << ((cur >> (i * 8)) & 0xFF);
        }
        return ss.str();
    }
};
}  // namespace gchrpc