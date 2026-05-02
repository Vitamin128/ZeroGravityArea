#pragma once
#include <ctime>
#include <iostream>
#include <string>

// 统一日志工具，供 gchrpc RPC 框架和 searcher 模块共同使用
namespace ns_util {

enum LogLevel { NORMAL = 1, WARNING, DEBUG, ERROR, FATAL };

inline std::string LevelToString(LogLevel level) {
    switch (level) {
        case NORMAL:  return "NORMAL";
        case WARNING: return "WARNING";
        case DEBUG:   return "DEBUG";
        case ERROR:   return "ERROR";
        case FATAL:   return "FATAL";
        default:      return "UNKNOWN";
    }
}

inline std::string GetTime() {
    time_t t = time(nullptr);
    struct tm *ct = localtime(&t);
    char buffer[128];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
             ct->tm_year + 1900, ct->tm_mon + 1, ct->tm_mday,
             ct->tm_hour, ct->tm_min, ct->tm_sec);
    return buffer;
}

}  // namespace ns_util

#define LOG(level)                                                                            \
    std::cout << "[" << ns_util::LevelToString(ns_util::level) << "][" << ns_util::GetTime() \
              << "][" << __FILE__ << ":" << __LINE__ << "] "
