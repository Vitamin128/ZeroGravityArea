#include <gchrpc/client/rpc_client.hpp>
#include <httplib.h>
#include <iostream>

int main() {
    // 1. 初始化 RPC Client，连接到后台的 RPC Server (8088)
    gchrpc::client::RpcClient rpc_client(false, "127.0.0.1", 8088);

    // 2. 初始化 HTTP Server (cpphttplib)
    httplib::Server svr;

    // 设置静态资源目录 (HTML/JS/CSS)
    svr.set_mount_point("/", "./wwwroot");

    // 3. 处理搜索请求：/search?word=xxx
    svr.Get("/search", [&](const httplib::Request& req, httplib::Response& res) {
        if (!req.has_param("word")) {
            res.set_content("Missing parameter 'word'", "text/plain");
            return;
        }
        std::string word = req.get_param_value("word");
        std::cout << "Received search request for: " << word << std::endl;

        // 构造 RPC 请求
        Json::Value rpc_req;
        rpc_req["query"] = word;
        Json::Value rpc_resp;

        // 调用后台 RPC Server 的 SearchService
        if (rpc_client.call("SearchService", rpc_req, rpc_resp)) {
            // 成功：将 RPC 响应 (Json::Value) 转为字符串返回给浏览器
            Json::StreamWriterBuilder builder;
            builder["emitUTF8"] = true;
            std::string out = Json::writeString(builder, rpc_resp);
            
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_content(out, "application/json");
        } else {
            // 失败
            res.status = 500;
            res.set_content("{\"error\": \"RPC call failed\"}", "application/json");
        }
    });

    std::cout << "HTTP Gateway started at http://0.0.0.0:8081" << std::endl;
    svr.listen("0.0.0.0", 8081);

    return 0;
}
