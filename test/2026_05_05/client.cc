#include <gchrpc/client/rpc_client.hpp>

void func3() {
    gchrpc::client::RpcClient client1(false, "127.0.0.1", 8088);
    Json::Value req;
    Json::Value resp;
    req["num1"] = 10;
    req["num2"] = 15;
    client1.call("Add", req, resp);
    std::cout << "result: " << resp["result"].asInt() << std::endl;
}
void func4() {
    gchrpc::client::RpcClient client1(false, "127.0.0.1", 8088);
    Json::Value req;
    Json::Value resp;
    std::string key;
    while (true) {
        std::cout << "Please Enter Query Key# ";  // 打印提示符
        if (!std::getline(std::cin, key)) break;  // 从终端读取一行，如果读取失败（如 Ctrl+D）则退出
        if (key.empty()) continue;                // 忽略空行
        req["query"] = key;                       // 使用输入的 key 作为查询词
        client1.call("SearchService", req, resp);
        std::cout << "result: " << resp.toStyledString() << std::endl;
    }
}
int main() {
    func4();
    return 0;
}
