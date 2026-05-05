#include <gchrpc/common/message.hpp>
#include <gchrpc/server/rpc_server.hpp>
#include <vector>

// #include "include/server/rpc_router.hpp"

// #include "include/server/rpc_router.hpp"
// #include <gchrpc/common/net.hpp>

void Add(const Json::Value &req, Json::Value &resp) {
    resp["result"] = req["num1"].asInt() + req["num2"].asInt();
}

void func() {
    std::vector<gchrpc::server::ServiceDescribe::ParamsDescribe> params;
    gchrpc::server::ServiceDescribe::ParamsDescribe p1("num1", gchrpc::server::VType::INTEGRAL);
    gchrpc::server::ServiceDescribe::ParamsDescribe p2("num2", gchrpc::server::VType::INTEGRAL);
    params.push_back(p1);
    params.push_back(p2);
    gchrpc::server::ServiceDescribe::ptr ptr = std::make_shared<gchrpc::server::ServiceDescribe>(
        "Add", std::move(params), gchrpc::server::VType::OBJECT, Add);
    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8088));
    server.RegistryMethod(ptr);
    server.Start();
}
int main() {
    func();
}
