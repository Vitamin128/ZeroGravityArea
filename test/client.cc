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
int main() {
    func3();
    return 0;
}
