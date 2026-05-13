#include <common/message.hpp>
#include <common/net.hpp>
#include <filesystem>
#include <iostream>
#include <searcher/searcher.hpp>
#include <server/rpc_server.hpp>
#include <thread>
#include <vector>

using namespace gchrpc::server;

int main() {
    // 实例化 RpcServer，并启用注册中心
    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8089), true,
                                     gchrpc::Address("127.0.0.1", 9090));

    ns_searcher::Searcher search_engine;

    // 初始化索引数据
    search_engine.BuildFullPDF("./InternalData/pdf", "./InternalData/pdf/raw.txt");
    search_engine.BuildFullHtml("./InternalData/html", "./InternalData/html/raw.txt");

    // ============================================================
    // 1. 注册 SearchService (搜索服务)
    // ============================================================
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("SearchService");
        factory.SetParamsDesc("query", VType::STRING);
        factory.SetReturnVtype(VType::ARRAY);
        factory.SetServiceDescribe(
            [&search_engine](const Json::Value &params, Json::Value &result) {
                std::string query = params["query"].asString();
                std::string json_str;
                search_engine.Search(query, &json_str);

                Json::Reader reader;
                if (!reader.parse(json_str, result) || !result.isArray()) {
                    result = Json::Value(Json::arrayValue);
                }
            });
        server.RegistryMethod(factory.build());
    }

    // ============================================================
    // 2. 注册 DownloadService (下载服务)
    // ============================================================
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("DownloadService");
        factory.SetParamsDesc("doc_id", VType::INTEGRAL);
        factory.SetReturnVtype(VType::STRING);
        factory.SetServiceDescribe(
            [&search_engine](const Json::Value &params, Json::Value &result) {
                uint64_t doc_id = params["doc_id"].asUInt64();
                result = search_engine.GetLocalPath(doc_id);
            });
        server.RegistryMethod(factory.build());
    }

    // ============================================================
    // 3. 注册 IndexService (动态索引服务)
    // ============================================================
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("IndexService");
        factory.SetParamsDesc("html_path", VType::STRING);
        factory.SetParamsDesc("pdf_path", VType::STRING);
        factory.SetReturnVtype(VType::OBJECT);
        factory.SetServiceDescribe(
            [&search_engine](const Json::Value &params, Json::Value &result) {
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

                // 处理 HTML
                auto html_files = get_files(html_path);
                if (html_files.size() == 1)
                    search_engine.AddSingleHtml(html_files[0]);
                else if (html_files.size() >= 2)
                    search_engine.BuildFullHtml(html_path, html_path + "/raw.txt");

                // 处理 PDF
                auto pdf_files = get_files(pdf_path);
                if (pdf_files.size() == 1)
                    search_engine.AddSinglePDF(pdf_files[0]);
                else if (pdf_files.size() >= 2)
                    search_engine.BuildFullPDF(pdf_path, pdf_path + "/raw.txt");

                result["status"] = "success";
            });
        server.RegistryMethod(factory.build());
    }

    std::cout << "Search Server started on 8088 using ServiceDescribeFactory..." << std::endl;
    server.Start();

    return 0;
}