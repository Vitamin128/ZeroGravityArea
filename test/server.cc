#include <gchrpc/common/message.hpp>
#include <gchrpc/common/net.hpp>
#include <gchrpc/searcher/search_engine.hpp>
#include <gchrpc/server/rpc_server.hpp>
#include <iostream>
#include <thread>
#include <vector>
void Search(const std::string &query, std::string *json_result, ns_engine::SearchEngine &engine) {
    engine.Search(query, json_result);
}
int main() {
    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8088));
    ns_engine::SearchEngine search_engine;
    // 假设数据路径正确
    search_engine.BuildFullPDF("/home/bamboo/ZeroGravityArea/pdfdata/fold",
                               "/home/bamboo/ZeroGravityArea/pdfdata/raw_html/raw.txt");
    search_engine.BuildFullHtml("/home/bamboo/ZeroGravityArea/data/input",
                                "/home/bamboo/ZeroGravityArea/data/raw_html/raw.txt");
    // 1. 定义输入参数（只需要 query）
    // 搜索方法注册
    gchrpc::server::ServiceDescribe::ParamsDescribe p1("query", gchrpc::server::VType::STRING);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params_search;
    params_search.push_back(p1);

    // 下载方法参数注册
    gchrpc::server::ServiceDescribe::ParamsDescribe p2("doc_id", gchrpc::server::VType::INTEGRAL);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params_download;
    params_download.push_back(p2);

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

    // 3. 初始化 ServiceDescribe，注册服务
    gchrpc::server::ServiceDescribe::ptr search_service =
        std::make_shared<gchrpc::server::ServiceDescribe>("SearchService", std::move(params_search),
                                                          gchrpc::server::VType::ARRAY,
                                                          search_handler);

    gchrpc::server::ServiceDescribe::ptr download_service =
        std::make_shared<gchrpc::server::ServiceDescribe>(
            "DownloadService", std::move(params_download), gchrpc::server::VType::STRING,
            download_handler);

    server.RegistryMethod(search_service);
    server.RegistryMethod(download_service);

    server.Start();
    return 0;
}