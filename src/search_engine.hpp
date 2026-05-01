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

    // 接口 4：增量更新（单篇 PDF 直接进内存）
    // 调用 Python AI 脚本解析 PDF，提取 <title>/<content> 后建立正倒排索引
    bool AddSinglePDF(const std::string &pdf_path) {
        std::string parsed_str;
        // 1. 用 AI 解析 PDF，拿到 title\3content\3path 格式字符串
        if (!ParserUtil::ParseSinglePDF(pdf_path, &parsed_str)) {
            LOG(WARNING) << "AddSinglePDF: Parse PDF failed: " << pdf_path << std::endl;
            return false;
        }

        // 2. 将解析结果写入正倒排索引
        if (!searcher.AddSingleDoc(parsed_str)) {
            LOG(WARNING) << "AddSinglePDF: Add to index failed: " << pdf_path << std::endl;
            return false;
        }

        LOG(NORMAL) << "AddSinglePDF: Successfully added to index: " << pdf_path << std::endl;
        return true;
    }

    // 接口 5：批量处理目录下所有 PDF 文件，逐一调用 AddSinglePDF 建立索引
    // pdf_dir 既可以是目录，也可以是单个 .pdf 文件路径
    bool BuildPdfIndex(const std::string &pdf_dir) {
        namespace fs = boost::filesystem;
        fs::path root_path(pdf_dir);
        if (!fs::exists(root_path)) {
            LOG(FATAL) << "BuildPdfIndex: path not exist: " << pdf_dir << std::endl;
            return false;
        }

        // 收集所有 .pdf 文件路径
        std::vector<std::string> pdf_files;
        if (fs::is_regular_file(root_path)) {
            if (root_path.extension() == ".pdf") {
                pdf_files.push_back(root_path.string());
            } else {
                LOG(WARNING) << "BuildPdfIndex: not a pdf file: " << pdf_dir << std::endl;
                return false;
            }
        } else {
            fs::recursive_directory_iterator end;
            for (fs::recursive_directory_iterator it(root_path); it != end; ++it) {
                if (fs::is_regular_file(*it) && it->path().extension() == ".pdf") {
                    pdf_files.push_back(it->path().string());
                }
            }
        }

        int success = 0;
        for (const auto &pdf : pdf_files) {
            if (AddSinglePDF(pdf)) {
                ++success;
            }
        }
        LOG(NORMAL) << "BuildPdfIndex: finished. success=" << success
                    << " / total=" << pdf_files.size() << std::endl;
        return success > 0 || pdf_files.empty();
    }

    // 接口 3：对外的搜索服务接口
    void Search(const std::string &query, std::string *json_result) {
        searcher.Search(query, json_result);
    }
};

}  // namespace ns_engine
