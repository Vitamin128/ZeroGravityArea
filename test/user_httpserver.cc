#include <httplib.h>
#include <chrono>
#include <client/rpc_client.hpp>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <jsoncpp/json/json.h>

int main() {
    // 1. 初始化 RPC Client
    gchrpc::client::RpcClient rpc_client(true, "127.0.0.1", 9090);

    // 2. 初始化 HTTP Server
    httplib::Server svr;

    // --- 接口 1: 用户注册 ---
    svr.Post("/api/register", [&](const httplib::Request &req, httplib::Response &res) {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(req.body, root)) {
            res.status = 400;
            return;
        }

        Json::Value params;
        params["username"] = root["username"].asString();
        params["password"] = root["password"].asString();
        params["nickname"] = root["nickname"].asString();

        Json::Value result;
        bool ok = rpc_client.call("UserRegister", params, result);
        
        res.set_header("Access-Control-Allow-Origin", "*");
        if (ok) res.set_content(result.toStyledString(), "application/json");
        else { res.status = 500; res.set_content("{\"status\":\"rpc_error\"}", "application/json"); }
    });

    // --- 接口 2: 用户登录 ---
    svr.Post("/api/login", [&](const httplib::Request &req, httplib::Response &res) {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(req.body, root)) {
            res.status = 400;
            return;
        }

        Json::Value params;
        params["username"] = root["username"].asString();
        params["password"] = root["password"].asString();

        Json::Value result;
        bool ok = rpc_client.call("UserLogin", params, result);

        res.set_header("Access-Control-Allow-Origin", "*");
        if (ok) res.set_content(result.toStyledString(), "application/json");
        else { res.status = 500; res.set_content("{\"status\":\"rpc_error\"}", "application/json"); }
    });

    // --- 接口 3: Token 校验 (自动登录) ---
    svr.Post("/api/verify_token", [&](const httplib::Request &req, httplib::Response &res) {
        Json::Value root;
        Json::Reader reader;
        if (!reader.parse(req.body, root)) {
            res.status = 400;
            return;
        }

        Json::Value params;
        params["token"] = root["token"].asString();

        Json::Value result;
        bool ok = rpc_client.call("UserVerifyToken", params, result);

        res.set_header("Access-Control-Allow-Origin", "*");
        if (ok) res.set_content(result.toStyledString(), "application/json");
        else { res.status = 500; res.set_content("{\"status\":\"rpc_error\"}", "application/json"); }
    });

    // --- 接口 4: 更新资料 ---
    svr.Post("/api/update_profile", [&](const httplib::Request &req, httplib::Response &res) {
        std::string user_id_str = req.get_file_value("user_id").content;
        std::string nickname = req.get_file_value("nickname").content;
        auto image_file = req.get_file_value("avatar");

        Json::Value params;
        params["user_id"] = (Json::Value::UInt64)std::stoull(user_id_str);
        params["nickname"] = nickname;
        params["image_data"] = image_file.content;

        Json::Value result;
        bool ok = rpc_client.call("UpdateUserProfile", params, result);

        res.set_header("Access-Control-Allow-Origin", "*");
        if (ok) res.set_content(result.toStyledString(), "application/json");
        else { res.status = 500; res.set_content("{\"status\":\"rpc_error\"}", "application/json"); }
    });

    // 跨域 OPTIONS
    svr.Options(R"(/api/.*)", [](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type, Authorization");
        res.status = 200;
    });

    std::cout << "HTTP User Gateway (Full Feature) started at http://0.0.0.0:8082" << std::endl;
    svr.listen("0.0.0.0", 8082);

    return 0;
}
