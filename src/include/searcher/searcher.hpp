#pragma once

#include <jsoncpp/json/json.h>

#include <algorithm>
#include <cctype>
#include <unordered_map>  // 新增：用于文档去重与权重合并
#include <unordered_set>  // 新增：用于查询词 O(1) 去重

#include "index.hpp"
#include "util.hpp"

namespace ns_searcher {

// 新增辅助结构体：用于在搜索时合并多个关键词命中的同一篇文档的权重
struct InvertedElemCombined {
    uint64_t doc_id;
    int weight;
    std::vector<std::string> words;  // 记录命中了哪些词，方便后续提取摘要
    InvertedElemCombined() : doc_id(0), weight(0) {}
};

class Searcher {
private:
    ns_index::Index *index;

public:
    Searcher() {
        index = ns_index::Index::GetInstance();
    }
    ~Searcher() {}

    // 加载索引容器
    void InitSearcher(std::string index_path) {
        if (index == nullptr) {
            LOG(FATAL) << "GetInstance failed" << std::endl;
            return;
        }

        // [修改前]：不检查构建结果
        // index->BuildIndex(index_path);

        //[修改后]：增加错误检查，避免索引构建失败仍假装成功
        if (!index->BuildIndex(index_path)) {
            LOG(FATAL) << "InitSearcher: BuildIndex failed for path: " << index_path << std::endl;
        }
    }

    // 暴露给上层的增量更新接口：将解析好的单行 HTML 格式化字符串插入内存索引
    bool AddSingleDoc(const std::string &line, ns_index::DocType type = ns_index::DocType::HTML) {
        if (index == nullptr) {
            LOG(WARNING) << "Searcher is not initialized!" << std::endl;
            return false;
        }
        return index->BuildSingleIndex(line, type);
    }

    // 搜索接口
    void Search(const std::string &query, std::string *json_result) {
        std::vector<std::string> words;
        ns_util::JiebaUtil::Cut(query, &words);

        // [修改前]：用 vector 存查询词，每次用 std::find 去重，复杂度 O(N)
        // std::vector<std::string> wordSame;

        // [修改后]：使用 unordered_set，哈希查找复杂度 O(1)，性能更高
        std::unordered_set<std::string> word_set;

        // [修改前]：把查到的所有倒排元素无脑 append
        // 到一起，导致不同关键词命中同一篇文档时，结果重复出现 ns_index::InvertedList
        // inverted_list_all;

        // [修改后]：使用 map，将相同 doc_id 的元素合并，实现文档去重并累加权重
        std::unordered_map<uint64_t, InvertedElemCombined> doc_map;

        // 遍历查询的每个词语，获取对应的倒排结果
        for (auto word : words) {
            boost::to_lower(word);

            // 查询词去重
            if (word_set.find(word) != word_set.end()) {
                continue;
            }
            word_set.insert(word);

            std::shared_ptr<ns_index::InvertedList> inverted_list = index->GetInvertedList(word);

            if (inverted_list == nullptr) {
                continue;
            }

            // 合并倒排列表到 doc_map 中
            for (const auto &elem : *inverted_list) {
                auto &combined_elem = doc_map[elem.doc_id];
                combined_elem.doc_id = elem.doc_id;
                combined_elem.weight += elem.weight;       // 关键：权重累加！
                combined_elem.words.push_back(elem.word);  // 记录它命中了哪些词
            }
        }

        // 将 map 中的合并结果转移到 vector 中，准备排序
        std::vector<InvertedElemCombined> inverted_list_all;
        for (const auto &pair : doc_map) {
            inverted_list_all.push_back(pair.second);
        }

        // 根据累加后的权重进行降序排序
        std::sort(inverted_list_all.begin(), inverted_list_all.end(),
                  [](const InvertedElemCombined &e1, const InvertedElemCombined &e2) {
                      return e1.weight > e2.weight;
                  });

        // 转换成 json 格式
        Json::Value root;
        for (const auto &item : inverted_list_all) {
            std::shared_ptr<ns_index::DocInfo> doc_info = index->GetForwardIndex(item.doc_id);
            if (doc_info == nullptr) {
                continue;
            }
            Json::Value elem;
            elem["title"] = doc_info->title;
            elem["local_path"] = doc_info->local_path;
            elem["type"] = static_cast<int>(doc_info->type);  // enum class 需显式转换为 int
            elem["weight"] = item.weight;
            // 提取摘要（传入第一个命中的关键词去定位即可）
            elem["content"] = GetDesc(doc_info->content, item.words[0]);
            root.append(elem);
        }

        // [修改前]：Json::StyledWriter 会生成大量回车和空格，极大地浪费网络带宽
        // Json::StyledWriter writer;

        //[修改后]：使用 FastWriter 压缩 JSON 体积，适合作为后端 RPC / Web API 响应
        Json::FastWriter writer;
        *json_result = writer.write(root);
    }

    std::string GetDesc(const std::string &content, const std::string &word) {
        auto iter =
            std::search(content.begin(), content.end(), word.begin(), word.end(),
                        [](char ch1, char ch2) { return std::tolower(ch1) == std::tolower(ch2); });
        size_t pos =
            (iter != content.end()) ? std::distance(content.begin(), iter) : std::string::npos;

        if (pos == std::string::npos) {
            if (content.size() <= 160) {
                return content;
            } else {
                return content.substr(0, 160) + "...";
            }
        } else {
            size_t begin = (pos >= 80) ? (pos - 80) : 0;
            size_t end = ((content.size() - pos) >= 80) ? (pos + 80) : content.size();

            // [修改前]：直接切片 `content.substr(begin, end - begin)`。
            // 致命隐患：如果是中文字符（UTF-8占3字节），切断在字符中间，前端会出现“”乱码，甚至导致
            // JSON 解析崩溃。

            // [修改后]：UTF-8 边界安全探测。
            // 原理：UTF-8 多字节字符的“延续字节”总是以二进制 `10xxxxxx` 开头（即 0x80 ~ 0xBF
            // 之间）。 只要位于这个区间，就说明切在了汉字中间，需要向前或向后对齐。

            // 修正 begin，向前寻找完整的字符起始位
            while (begin > 0 && (content[begin] & 0xC0) == 0x80) {
                begin--;
            }
            // 修正 end，向后寻找完整的字符起始位
            while (end < content.size() && (content[end] & 0xC0) == 0x80) {
                end++;
            }

            std::string desc = content.substr(begin, end - begin);
            if (begin != 0) {
                desc = "..." + desc;
            }
            if (end != content.size()) {
                desc = desc + "...";
            }
            return desc;
        }
    }
};

}  // namespace ns_searcher