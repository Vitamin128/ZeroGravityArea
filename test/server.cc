#include <filesystem>
#include <common/message.hpp>
#include <common/net.hpp>
#include <searcher/searcher.hpp>
#include <server/rpc_server.hpp>
#include <iostream>
#include <thread>
#include <vector>
void Search(const std::string &query, std::string *json_result, ns_searcher::Searcher &engine) {
    engine.Search(query, json_result);
}
int main() {
    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8088), true,
                                     gchrpc::Address("127.0.0.1", 9090));
    ns_searcher::Searcher search_engine;
    // 假设数据路径正确
    search_engine.BuildFullPDF("./InternalData/pdf", "./InternalData/pdf/raw.txt");
    search_engine.BuildFullHtml("./InternalData/html", "./InternalData/html/raw.txt");
    // 1. 定义输入参数（只需要 query）
    // 搜索方法注册
    gchrpc::server::ServiceDescribe::ParamsDescribe p1("query", gchrpc::server::VType::STRING);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params_search;
    params_search.push_back(p1);

    // 下载方法参数注册
    gchrpc::server::ServiceDescribe::ParamsDescribe p2("doc_id", gchrpc::server::VType::INTEGRAL);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params_download;
    params_download.push_back(p2);

    // 索引方法参数注册
    gchrpc::server::ServiceDescribe::ParamsDescribe p3("html_path", gchrpc::server::VType::STRING);
    gchrpc::server::ServiceDescribe::ParamsDescribe p4("pdf_path", gchrpc::server::VType::STRING);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params_index;
    params_index.push_back(p3);
    params_index.push_back(p4);

    // 2. 使用 Lambda 包装搜索逻辑
    auto search_handler = [&search_engine](const Json::Value &params, Json::Value &result) {
        std::string query = params["query"].asString();
        std::string json_str;
        // 调用搜索引擎
        search_engine.Search(query, &json_str);
        // 将搜索结果字符串解析回 Json::Value 对象返回给客户端
        Json::Reader reader;
        if (!reader.parse(json_str, result)) {
            LOG(ERROR) << "Failed to parse search result to JSON" << std::endl;
            result = Json::Value(Json::arrayValue);
        }
        // 兜底校验：确保即便解析成功，result 也必须是一个数组以符合 VType::ARRAY
        if (!result.isArray()) {
            result = Json::Value(Json::arrayValue);
        }
    };

    auto download_handler = [&search_engine](const Json::Value &params, Json::Value &result) {
        uint64_t doc_id = params["doc_id"].asUInt64();
        std::string local_path = search_engine.GetLocalPath(doc_id);

        // 直接将文件路径作为结果返回 (字符串类型)
        result = local_path;
    };

    auto index_handler = [&search_engine](const Json::Value &params, Json::Value &result) {
        std::string html_path = params["html_path"].asString();
        std::string pdf_path = params["pdf_path"].asString();

        auto get_files = [](const std::string &dir) {
            std::vector<std::string> files;
            if (std::filesystem::exists(dir) && std::filesystem::is_directory(dir)) {
                for (const auto &entry : std::filesystem::directory_iterator(dir)) {
                    if (entry.is_regular_file()) files.push_back(entry.path().string());
                }
            }
            return files;
        };

        // 1. 处理 HTML 目录
        auto html_files = get_files(html_path);
        if (html_files.size() == 1) {
            search_engine.AddSingleHtml(html_files[0]);
        } else if (html_files.size() >= 2) {
            search_engine.BuildFullHtml(html_path, html_path + "/raw.txt");
        }

        // 2. 处理 PDF 目录
        auto pdf_files = get_files(pdf_path);
        if (pdf_files.size() == 1) {
            search_engine.AddSinglePDF(pdf_files[0]);
        } else if (pdf_files.size() >= 2) {
            search_engine.BuildFullPDF(pdf_path, pdf_path + "/raw.txt");
        }
        result["status"] = "success";
    };

    // 3. 初始化 ServiceDescribe，注册服务
    gchrpc::server::ServiceDescribe::ptr search_service =
        std::make_shared<gchrpc::server::ServiceDescribe>("SearchService", std::move(params_search),
                                                          gchrpc::server::VType::ARRAY,
                                                          search_handler);

    gchrpc::server::ServiceDescribe::ptr download_service =
        std::make_shared<gchrpc::server::ServiceDescribe>(
            "DownloadService", std::move(params_download), gchrpc::server::VType::STRING,
            download_handler);

    gchrpc::server::ServiceDescribe::ptr index_service =
        std::make_shared<gchrpc::server::ServiceDescribe>(
            "IndexService", std::move(params_index), gchrpc::server::VType::OBJECT, index_handler);

    server.RegistryMethod(search_service);
    server.RegistryMethod(download_service);
    server.RegistryMethod(index_service);

    server.Start();
    return 0;
}