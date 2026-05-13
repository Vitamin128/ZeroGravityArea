#include <httplib.h>

#include <chrono>
#include <client/rpc_client.hpp>
#include <ctime>
#include <filesystem>  // 新增
#include <fstream>     // 新增
#include <iostream>
#include <iterator>  // 新增
#include <unordered_map>

int main() {
    // 1. 初始化 RPC Client，连接本地 RPC Server (8088)
    gchrpc::client::RpcClient rpc_client(true, "127.0.0.1", 9090);

    // 2. 初始化 HTTP Server (cpphttplib)
    httplib::Server svr;

    std::cout << "HTTP Gateway started at http://0.0.0.0:8081" << std::endl;
    svr.listen("0.0.0.0", 8082);

    return 0;
}
