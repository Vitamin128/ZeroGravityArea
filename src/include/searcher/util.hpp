#pragma once

#include "../common/logger.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// 匹配你提供的头文件路径
#include <httplib.h>

#include "/home/bamboo/ZeroGravityArea/cppjieba/include/cppjieba/Jieba.hpp"

namespace ns_util {

class FileUtil {
public:
    // 读取文件内容到字符串
    static bool ReadFile(const std::string &file_path, std::string *content) {
        std::ifstream file(file_path, std::ios::in);
        if (!file.is_open()) {
            LOG(FATAL) << "open file error! file: " << file_path << std::endl;
            return false;
        }
        std::string line;
        // 注意：这里保留了逐行读取，如果预处理文件是用 \n 分隔文档的，这是正确的
        while (std::getline(file, line)) {
            *content += line;
        }
        file.close();
        return true;
    }

    // 调用 Python 脚本解析 PDF 文件 (修改为使用 httplib 直接请求本地服务)
    static bool ParsePDF(const std::string &pdf_path, std::string *content) {
        if (!content) return false;

        // 1. 创建 HTTP 客户端对象，连接本地服务
        httplib::Client cli("http://127.0.0.1:8080");
        cli.set_read_timeout(60, 0);  // 设置读取超时为 60 秒 (Gemini 解析 PDF 耗时较长)

        // 2. 构造 JSON 请求体
        // 注意：这里我们手动拼接 JSON。对于复杂场景建议使用 nlohmann/json 库
        std::string json_body = "{\"path\":\"" + pdf_path + "\"}";

        // 3. 发起 POST 请求
        if (auto res = cli.Post("/", json_body, "application/json")) {
            // 4. 检查响应状态码
            if (res->status == 200) {
                *content = res->body;

                // 额外检查一下业务逻辑层是否返回了错误信息
                if (content->find("Error:") != std::string::npos) {
                    LOG(FATAL) << "Python service returned logic error: " << *content << std::endl;
                    return false;
                }
                return true;
            } else {
                LOG(FATAL) << "HTTP request failed with status: " << res->status
                           << ", body: " << res->body << std::endl;
                return false;
            }
        } else {
            // 请求发送失败（比如服务没启动）
            auto err = res.error();
            LOG(FATAL) << "HTTP connection failed. Error code: " << (int)err
                       << ". Is the Python service running on port 8080?" << std::endl;
            return false;
        }
    }
};

class StringUtil {
public:
    // 字符串分割函数通过 \3 分割
    static void Split(const std::string &s, std::string sep, std::vector<std::string> *elems) {
        // boost::token_compress_on: 如果有两个分隔符连在一起，是否压缩（通常设为 off 比较严谨）
        boost::split(*elems, s, boost::is_any_of(sep), boost::token_compress_on);
    }
};

// 匹配你提供的字典路径
const char *const DICT_PATH = "/home/bamboo/ZeroGravityArea/cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "/home/bamboo/ZeroGravityArea/cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "/home/bamboo/ZeroGravityArea/cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "/home/bamboo/ZeroGravityArea/cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "/home/bamboo/ZeroGravityArea/cppjieba/dict/stop_words.utf8";

class JiebaUtil {
private:
    // 使用函数内的静态局部变量（Meyers' Singleton）来代替类的静态成员变量
    // C++11 保证了静态局部变量的初始化是线程安全的，且只会被初始化一次。
    static cppjieba::Jieba &GetJieba() {
        static cppjieba::Jieba jieba(DICT_PATH, HMM_PATH, USER_DICT_PATH, IDF_PATH, STOP_WORD_PATH);
        return jieba;
    }

public:
    // 1. 普通分词：用于对用户查询的搜索词进行初步切分
    static void Cut(const std::string &sentence, std::vector<std::string> *words) {
        GetJieba().Cut(sentence, *words);
    }

    // 2. 搜索引擎模式分词：用于构建倒排索引
    // 建议在 index.hpp 构建索引时调用这个方法
    static void CutForSearch(const std::string &sentence, std::vector<std::string> *words) {
        GetJieba().CutForSearch(sentence, *words);
    }
};
}  // namespace ns_util