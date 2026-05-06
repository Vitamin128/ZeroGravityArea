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
    ns_engine::SearchEngine search_engine;
    // 假设数据路径正确
    search_engine.BuildFullPDF("/home/bamboo/ZeroGravityArea/pdfdata/fold",
                               "/home/bamboo/ZeroGravityArea/pdfdata/raw_html/raw.txt");
    // 1. 定义输入参数（只需要 query）
    gchrpc::server::ServiceDescribe::ParamsDescribe p1("query", gchrpc::server::VType::STRING);
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params;
    params.push_back(p1);
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
    // 3. 初始化 ServiceDescribe，注意返回类型改为 ARRAY
    gchrpc::server::ServiceDescribe::ptr search_service =
        std::make_shared<gchrpc::server::ServiceDescribe>(
            "SearchService", std::move(params), gchrpc::server::VType::ARRAY, search_handler);

    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8088));
    server.RegistryMethod(search_service);
    server.Start();
    return 0;
}