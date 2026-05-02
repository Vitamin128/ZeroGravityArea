#include <iostream>
#include <string>

#include "search_engine.hpp"
int func() {
    ns_engine::SearchEngine engine;

    std::string html_dir = "../data/input";               // 原始 HTML 所在的目录
    std::string raw_path = "../data_1/raw_html/raw.txt";  // 清洗后的输出路径

    // 1. 全量构建：一键清洗目录下的所有文件并建立索引
    engine.BuildFullHtml(html_dir, raw_path);

    // 2. 增量更新示例：假如后续又来了一篇新文章，一键秒加进内存！
    engine.AddSingleHtml("/home/bamboo/boost-search-engine/data_1/input/acknowledgements.html");

    // 3. 开启搜索交互循环
    std::string query;
    std::cout << "===========================================" << std::endl;
    std::cout << "       欢迎使用 Boost 离线搜索引擎" << std::endl;
    std::cout << "===========================================" << std::endl;
    std::cout << "请输入搜索关键词 (输入 exit 退出): ";

    while (std::getline(std::cin, query)) {
        if (query == "exit" || query == "quit") {
            std::cout << "感谢使用，再见！" << std::endl;
            break;
        }

        if (query.empty()) {
            std::cout << "搜索词不能为空，请重新输入: ";
            continue;
        }

        std::string json_result;
        engine.Search(query, &json_result);

        std::cout << "\n---------------- 搜索结果 ----------------\n";
        std::cout << json_result << std::endl;
        std::cout << "------------------------------------------\n\n";

        std::cout << "请输入搜索关键词 (输入 exit 退出): ";
    }

    return 0;
}
void func0() {
    std::string content;
    ns_util::FileUtil::ParsePDF("/home/bamboo/boost-search-engine/ganchuhao.pdf", &content);
    LOG(NORMAL) << content << std::endl;
}
void func1() {
    ns_engine::SearchEngine engine;

    // 1. 指定测试用的 PDF 路径
    std::string pdf_path = "/home/bamboo/ZeroGravityArea/ganchuhao.pdf";

    LOG(NORMAL) << "--- 开始测试 PDF 索引接口 ---" << std::endl;

    // 2. 测试单篇 PDF 增量构建索引
    if (engine.AddSinglePDF(pdf_path)) {
        LOG(NORMAL) << "PDF 索引构建成功: " << pdf_path << std::endl;
    } else {
        LOG(WARNING) << "PDF 索引构建失败，请检查 Python 服务是否启动" << std::endl;
        return;
    }

    // 3. 测试搜索功能，验证 PDF 内容是否可搜索
    std::string query = "甘初豪";
    std::string json_result;
    engine.Search(query, &json_result);

    LOG(NORMAL) << "搜索关键词 [" << query << "] 的结果:" << std::endl;
    std::cout << json_result << std::endl;
}

int main() {
    func1();
    return 0;
}