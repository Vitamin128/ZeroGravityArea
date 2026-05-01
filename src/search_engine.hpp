#pragma once

#include <iostream>
#include <string>

#include "parser.hpp"
#include "searcher.hpp"

namespace ns_engine {

class SearchEngine {
private:
    ns_searcher::Searcher searcher;

public:
    SearchEngine() {}
    ~SearchEngine() {}

    // 接口 1：全量构建（包含洗数据 + 建索引）
    // 将指定文件夹下的 html 文件先洗进 raw_path 中，然后再拿出来构建正倒排索引
    bool BuildFullIndex(const std::string &html_dir, const std::string &raw_path) {
        // 1. 数据清洗 (解析 HTML -> 保存至 raw.txt)
        LOG(NORMAL) << "Start building full index from: " << html_dir << std::endl;
        int ret = ParserUtil::parserprocess(html_dir, raw_path);
        if (ret != 0) {
            LOG(FATAL) << "Parser process failed with code: " << ret << std::endl;
            return false;
        }

        // 2. 根据 raw.txt 初始化搜索器（构建正排和倒排索引）
        searcher.InitSearcher(raw_path);
        LOG(NORMAL) << "Full index build complete!" << std::endl;
        return true;
    }

    // 接口 2：增量更新（单篇 html 直接进内存）
    // 指定 html 文件的路径，不洗进 raw_path，直接更新正倒排索引
    bool AddSingleHtml(const std::string &html_path) {
        std::string parsed_str;
        // 1. 解析单篇 HTML，拿到格式化好的字符串
        if (!ParserUtil::ParseSingleFile(html_path, &parsed_str)) {
            LOG(WARNING) << "AddSingleHtml: Parse single file failed: " << html_path << std::endl;
            return false;
        }

        // 2. 直接将这一行塞进内存中的倒排和正排索引
        if (!searcher.AddSingleDoc(parsed_str)) {
            LOG(WARNING) << "AddSingleHtml: Add to index failed: " << html_path << std::endl;
            return false;
        }

        LOG(NORMAL) << "AddSingleHtml: Successfully added to index: " << html_path << std::endl;
        return true;
    }

    // 接口 3：对外的搜索服务接口
    void Search(const std::string &query, std::string *json_result) {
        searcher.Search(query, json_result);
    }
};

}  // namespace ns_engine
