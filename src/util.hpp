#pragma once

#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
// 匹配你提供的头文件路径
#include "../cppjieba/include/cppjieba/Jieba.hpp"

namespace ns_util {

class FileUtil {
public:
    // 读取文件内容到字符串
    static bool ReadFile(const std::string &file_path, std::string *content) {
        std::ifstream file(file_path, std::ios::in);
        if (!file.is_open()) {
            std::cerr << "open file error! file: " << file_path << std::endl;
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

class ParserUtil {
private:
public:
    struct DocInfo {
        std::string title;
        std::string content;
        std::string local_path;
    };

    // 将src_path文件夹下面的每个html文件的路径存储起来
    static bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list) {
        namespace fs = boost::filesystem;
        fs::path root_path(src_path);
        if (!fs::exists(root_path)) {
            std::cout << root_path << " path not exist!" << std::endl;
            return false;
        }
        // --- 新增逻辑：如果传进来的是个文件，直接放进列表即可 ---
        if (fs::is_regular_file(root_path)) {
            if (root_path.extension() == ".html") {
                files_list->push_back(root_path.string());
            }
            return true;
        }
        fs::recursive_directory_iterator end;
        for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++) {
            if (!fs::is_regular_file(*iter)) {
                continue;
            }
            if (iter->path().extension() != ".html") {
                continue;
            }
            // cout<<"debug: "<<iter->path().string()<<endl;
            files_list->push_back(iter->path().string());
        }
        return true;
    }
    static bool ParseTitle(const std::string &html, std::string *title) {
        int begin = html.find("<title>");
        if (begin == std::string::npos) {
            return false;
        }
        begin += strlen("<title>");
        int end = html.find("</title>");
        if (end == std::string::npos) {
            return false;
        }
        if (end < begin) {
            return false;
        }
        *title = html.substr(begin, end - begin);
        // cout<<"debug: title= "<<*title<<endl;
        return true;
    }
    static bool ParseContent(const std::string &html, std::string *content) {
        enum class State { LABEL, CONTENT };
        State state = State::LABEL;
        for (char c : html) {
            switch (state) {
                case State::LABEL:
                    if (c == '>') {
                        state = State::CONTENT;
                    }
                    break;
                case State::CONTENT:
                    if (c == '<') {
                        state = State::LABEL;
                    } else {
                        if (c == '\n') {
                            content->push_back(' ');
                        } else {
                            content->push_back(c);
                        }
                    }
                    break;
                default:
                    break;
            }
        }
        return true;
    }
    static void ShowDoc(const DocInfo &doc) {
        std::cout << "title: " << doc.title << std::endl;
        std::cout << "content: " << doc.content << std::endl;
    }
    static bool ParseHtml(const std::vector<std::string> &files_list, std::vector<DocInfo> *results,
                          const std::string &src_path) {
        for (const std::string &file : files_list) {
            std::string result;

            // 讲file对应目录文件下的内容写入到string result字符串中
            if (!ns_util::FileUtil::ReadFile(file, &result)) {
                std::cout << "read file error! file: " << file << std::endl;
                continue;
            }
            // cout<<"debug result: "<<result<<endl;
            DocInfo doc;
            if (!ParseTitle(result, &doc.title)) {
                std::cout << "parse title error! file: " << file << std::endl;
                continue;
            }
            if (!ParseContent(result, &doc.content)) {
                std::cout << "parse content error! file: " << file << std::endl;
                continue;
            }
            doc.local_path = file;
            results->push_back(
                std::move(doc));  // move会将doc的内容搬运到results中，避免拷贝，提高效率
        }
        return true;
    }
    static bool SaveHtml(const std::vector<DocInfo> &results, const std::string &output) {
#define SEP '\3'
        std::ofstream out(output, std::ios::out | std::ios::binary);
        if (!out.is_open()) {
            std::cerr << "open " << output << " failed!" << std::endl;
            return false;
        }
        for (auto &item : results) {
            out << item.title << SEP << item.content << SEP << item.local_path << '\n';
        }
        out.close();
        return true;
    }
};
}  // namespace ns_util