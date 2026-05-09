#pragma once

#include "util.hpp"

class ParserUtil {
public:
    static int parserprocess(const std::string &src_path, const std::string &output_path) {
        std::vector<std::string> files_list;
        if (!EnumFile(src_path, &files_list, ".html")) {
            LOG(FATAL) << "enum file name error!" << std::endl;
            return 1;
        }

        std::vector<DocInfo> results;
        if (!ParseHtml(files_list, &results)) {
            LOG(FATAL) << "parse html error!" << std::endl;
            return 2;
        }

        if (!SaveHtml(results, output_path)) {
            LOG(FATAL) << "Save html error!" << std::endl;
            return 3;
        }

        LOG(NORMAL) << "parse html success!" << std::endl;
        return 0;
    }

    // 新增：只解析单个 HTML 文件，并将结果格式化为内部索引需要的单行 string 返回
    static bool ParseSingleFile(const std::string &file_path, std::string *out_string) {
        std::string result;
        if (!ns_util::FileUtil::ReadFile(file_path, &result)) {
            LOG(WARNING) << "read file error! file: " << file_path << std::endl;
            return false;
        }

        DocInfo doc;
        // 既然 HTML 也要用文件名作为标题
        namespace fs = boost::filesystem;
        doc.title = fs::path(file_path).filename().string();

        if (!ParseContent(result, &doc.content)) {
            LOG(WARNING) << "parse content error! file: " << file_path << std::endl;
            return false;
        }
        doc.local_path = file_path;

        // 组装成和 raw.txt 里面一模一样的格式 (注意：不带末尾的换行符 \n)
        *out_string = doc.title + '\3' + doc.content + '\3' + doc.local_path;
        return true;
    }

    // 新增：解析单个 PDF 文件
    // 调用 Python AI 脚本获取带 <title>/<content> 标签的字符串，
    // 直接复用私有方法 ParseTitle / ParseContent 提取内容，
    // 最终组装成与 ParseSingleFile 相同的 \3 分隔格式（title\3content\3path）
    static bool ParseSinglePDF(const std::string &pdf_path, std::string *out_string) {
        // 1. 调用 Python 脚本，让 AI 解析 PDF 并返回带 <title>/<content> 标签的字符串
        std::string ai_output;
        if (!ns_util::FileUtil::ParsePDF(pdf_path, &ai_output)) {
            LOG(WARNING) << "ParseSinglePDF: ParsePDF failed for: " << pdf_path << std::endl;
            return false;
        }

        // 2. 既然 AI 不再输出标签，我们使用 PDF 文件名作为标题
        namespace fs = boost::filesystem;
        std::string title = fs::path(pdf_path).filename().string();

        // 3. 将 AI 还原的全文作为 content，并将换行符替换为空格（保持单行格式）
        std::string content = ai_output;
        std::replace(content.begin(), content.end(), '\n', ' ');

        if (title.empty() || content.empty()) {
            LOG(WARNING) << "ParseSinglePDF: empty title or content for: " << pdf_path << std::endl;
            return false;
        }

        // 4. 组装成标准格式：title\3content\3local_path
        *out_string = title + '\3' + content + '\3' + pdf_path;
        return true;
    }

    // 新增：PDF 全量清洗接口
    static int pdfprocess(const std::string &src_path, const std::string &output_path) {
        std::vector<std::string> files_list;
        if (!EnumFile(src_path, &files_list, ".pdf")) {
            LOG(FATAL) << "enum pdf file error!" << std::endl;
            return 1;
        }

        std::ofstream out(output_path, std::ios::out | std::ios::binary);
        if (!out.is_open()) {
            LOG(FATAL) << "open " << output_path << " failed!" << std::endl;
            return 2;
        }

        int total = files_list.size();
        int processed_count = 0;
        int count = 0;
        for (const auto &file : files_list) {
            processed_count++;
            LOG(NORMAL) << "Processing PDF [" << processed_count << "/" << total << "]: " << file
                        << std::endl;
            std::string out_string;
            if (ParseSinglePDF(file, &out_string)) {
                out << out_string << "\n";
                count++;
            }
        }
        out.close();
        LOG(NORMAL) << "\nPDF parse and save success! Total: " << count << std::endl;
        return 0;
    }

private:
    struct DocInfo {
        std::string title;
        std::string content;
        std::string local_path;
    };
    // void
    // 将src_path文件夹下面的每个html文件的路径存储起来
    static bool EnumFile(const std::string &src_path, std::vector<std::string> *files_list,
                         const std::string &ext = ".html") {
        namespace fs = boost::filesystem;
        fs::path root_path(src_path);
        if (!fs::exists(root_path)) {
            LOG(WARNING) << root_path << " path not exist!" << std::endl;
            return false;
        }
        // --- 新增逻辑：如果传进来的是个文件，直接放进列表即可 ---
        if (fs::is_regular_file(root_path)) {
            if (root_path.extension() == ext) {
                files_list->push_back(root_path.string());
            }
            return true;
        }
        fs::recursive_directory_iterator end;
        for (fs::recursive_directory_iterator iter(root_path); iter != end; iter++) {
            if (!fs::is_regular_file(*iter)) {
                continue;
            }
            if (iter->path().extension() != ext) {
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
        std::cout << "local_path: " << doc.local_path << std::endl;
    }
    static bool ParseHtml(const std::vector<std::string> &files_list,
                          std::vector<DocInfo> *results) {
        for (const std::string &file : files_list) {
            std::string result;

            // 讲file对应目录文件下的内容写入到string result字符串中
            if (!ns_util::FileUtil::ReadFile(file, &result)) {
                LOG(WARNING) << "read file error! file: " << file << std::endl;
                continue;
            }
            // cout<<"debug result: "<<result<<endl;
            DocInfo doc;
            // 批量处理时同样使用文件名作为标题
            namespace fs = boost::filesystem;
            doc.title = fs::path(file).filename().string();

            if (!ParseContent(result, &doc.content)) {
                LOG(WARNING) << "parse content error! file: " << file << std::endl;
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
            LOG(FATAL) << "open " << output << " failed!" << std::endl;
            return false;
        }
        for (auto &item : results) {
            out << item.title << SEP << item.content << SEP << item.local_path << '\n';
        }
        out.close();
        return true;
    }
};
