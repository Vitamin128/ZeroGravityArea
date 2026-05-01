#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// 匹配你提供的头文件路径
#include "../cppjieba/include/cppjieba/Jieba.hpp"
#include <ctime>

namespace ns_util {

enum LogLevel {
    NORMAL = 1,
    WARNING,
    DEBUG,
    FATAL
};

inline std::string LevelToString(LogLevel level) {
    switch (level) {
        case NORMAL: return "NORMAL";
        case WARNING: return "WARNING";
        case DEBUG: return "DEBUG";
        case FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

inline std::string GetTime() {
    time_t t = time(nullptr);
    struct tm *ctime = localtime(&t);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             ctime->tm_year + 1900, ctime->tm_mon + 1, ctime->tm_mday,
             ctime->tm_hour, ctime->tm_min, ctime->tm_sec);
    return buffer;
}

} // namespace ns_util

#define LOG(level) std::cout << "[" << ns_util::LevelToString(ns_util::level) << "][" \
                             << ns_util::GetTime() << "][" << __FILE__ << ":" << __LINE__ << "] "

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
const char *const DICT_PATH = "../cppjieba/dict/jieba.dict.utf8";
const char *const HMM_PATH = "../cppjieba/dict/hmm_model.utf8";
const char *const USER_DICT_PATH = "../cppjieba/dict/user.dict.utf8";
const char *const IDF_PATH = "../cppjieba/dict/idf.utf8";
const char *const STOP_WORD_PATH = "../cppjieba/dict/stop_words.utf8";

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