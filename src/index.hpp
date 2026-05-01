#pragma once

#include <fstream>
#include <iostream>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "util.hpp"

namespace ns_index {

// 文档类型枚举：区分 HTML 和 PDF
enum class DocType {
    HTML = 0,
    PDF  = 1
};

// 正排索引结构体
struct DocInfo {
    uint64_t doc_id;
    std::string title;
    std::string content;
    std::string local_path;
    DocType type = DocType::HTML;  // 默认为 HTML
};

// 倒排索引元素
struct InvertedElem {
    uint64_t doc_id;
    std::string word;
    int weight;
};

// 🌟 重点修改 1：InvertedList 现在是一个 vector，我们将管理它的 shared_ptr
typedef std::vector<InvertedElem> InvertedList;

class Index {
private:
    // 正排索引：存储 DocInfo 的智能指针
    std::vector<std::shared_ptr<DocInfo>> forward_index;

    // 🌟 重点修改 2：倒排索引：key 是词，value 是指向 vector 的智能指针
    std::unordered_map<std::string, std::shared_ptr<InvertedList>> inverted_index;

    // 读写锁，保护两个索引容器
    mutable std::shared_mutex rw_mtx;

    Index() {}
    Index(const Index &) = delete;
    Index &operator=(const Index &) = delete;
    ~Index() {}

public:
    static Index *GetInstance() {
        static Index instance;
        return &instance;
    }

    // 正排查询：返回 DocInfo 的智能指针
    std::shared_ptr<DocInfo> GetForwardIndex(uint64_t doc_id) const {
        std::shared_lock<std::shared_mutex> lock(rw_mtx);
        if (doc_id >= forward_index.size()) {
            return nullptr;
        }
        return forward_index[doc_id];
    }

    // 🌟 重点修改 3：倒排查询：返回 InvertedList 的智能指针
    // 这样外部获取后，即便 map 发生了 rehash 或 vector 发生了扩容，数据依然安全
    std::shared_ptr<InvertedList> GetInvertedList(const std::string &word) const {
        std::shared_lock<std::shared_mutex> lock(rw_mtx);
        auto it = inverted_index.find(word);
        if (it != inverted_index.end()) {
            return it->second;  // 仅拷贝智能指针，引用计数+1，O(1) 效率
        }
        return nullptr;
    }

    bool BuildIndex(const std::string &input) {
        std::fstream in(input, std::ios::in | std::ios::binary);
        if (!in.is_open()) {
            LOG(FATAL) << "open file error" << std::endl;
            return false;
        }

        std::string file;
        int count = 0;
        while (std::getline(in, file)) {
            std::shared_ptr<DocInfo> doc = BuildForwardIndex(file);
            if (doc == nullptr) continue;

            BuildInvertedIndex(doc);

            count++;
            if (count % 50 == 0) {
                LOG(NORMAL) << "build index doc count: " << count << '\r' << std::flush;
            }
        }
        std::cout << std::endl;
        return true;
    }

    // 新增：增量更新接口，直接在内存中将单篇已经格式化好的字符串（三字段 \3 分隔）加入索引
    bool BuildSingleIndex(const std::string &line) {
        std::shared_ptr<DocInfo> doc = BuildForwardIndex(line);
        if (doc == nullptr) {
            return false;
        }
        return BuildInvertedIndex(doc);
    }

private:
    std::shared_ptr<DocInfo> BuildForwardIndex(const std::string &line) {
        std::vector<std::string> results;
        std::string sep = "\3";
        ns_util::StringUtil::Split(line, sep, &results);
        if (results.size() != 3) return nullptr;

        auto doc = std::make_shared<DocInfo>();
        doc->title = results[0];
        doc->content = results[1];
        doc->local_path = results[2];

        std::unique_lock<std::shared_mutex> lock(rw_mtx);
        doc->doc_id = forward_index.size();
        forward_index.push_back(doc);

        return doc;
    }

    // 🌟 重点修改 4：构建倒排时的智能指针逻辑
    bool BuildInvertedIndex(std::shared_ptr<DocInfo> doc) {
        struct word_cnt {
            int title_cnt;
            int content_cnt;
            word_cnt() : title_cnt(0), content_cnt(0) {}
        };
        std::unordered_map<std::string, word_cnt> word_map;

        // 1. 在锁外进行耗时的分词操作
        std::vector<std::string> title_words;
        ns_util::JiebaUtil::Cut(doc->title, &title_words);
        for (const auto &word : title_words) word_map[word].title_cnt++;

        std::vector<std::string> content_words;
        ns_util::JiebaUtil::Cut(doc->content, &content_words);
        for (const auto &word : content_words) word_map[word].content_cnt++;

        // 2. 写入全局索引时加写锁
        std::unique_lock<std::shared_mutex> lock(rw_mtx);
        for (const auto &word_pair : word_map) {
            InvertedElem elem;
            elem.doc_id = doc->doc_id;
            elem.word = word_pair.first;
            elem.weight = word_pair.second.title_cnt * 10 + word_pair.second.content_cnt;

            // 获取或创建该词对应的 vector 智能指针
            auto &list_ptr = inverted_index[elem.word];
            if (!list_ptr) {
                // 如果该词第一次出现，分配一个新的 vector
                list_ptr = std::make_shared<InvertedList>();
            }
            list_ptr->push_back(std::move(elem));
        }
        return true;
    }
};
}  // namespace ns_index