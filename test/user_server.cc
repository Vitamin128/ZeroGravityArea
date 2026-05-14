#include <jsoncpp/json/json.h>

#include <common/message.hpp>
#include <common/net.hpp>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <server/rpc_server.hpp>
#include <user/user.hpp>

using namespace gchrpc::server;

int main() {
    gchrpc::server::RpcServer server(gchrpc::Address("127.0.0.1", 8088), true,
                                     gchrpc::Address("127.0.0.1", 9090));

    user::UserManager user_manager;

    // 1. 注册接口: UserRegister
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("UserRegister");
        factory.SetParamsDesc("username", VType::STRING);
        factory.SetParamsDesc("password", VType::STRING);
        factory.SetParamsDesc("nickname", VType::STRING);
        factory.SetReturnVtype(VType::BOOL);
        factory.SetServiceDescribe([&](const Json::Value &params, Json::Value &result) {
            std::string u = params["username"].asString();
            std::string p = params["password"].asString();
            std::string n = params["nickname"].asString();
            result = user_manager.Register(u, p, n);
        });
        server.RegistryMethod(factory.build());
    }

    // 2. 登录接口: UserLogin
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("UserLogin");
        factory.SetParamsDesc("username", VType::STRING);
        factory.SetParamsDesc("password", VType::STRING);
        factory.SetReturnVtype(VType::OBJECT);
        factory.SetServiceDescribe([&](const Json::Value &params, Json::Value &result) {
            std::string u = params["username"].asString();
            std::string p = params["password"].asString();
            auto res = user_manager.Login(u, p);
            if (res) {
                result["status"] = "success";
                result["token"] = res->current_token;
                result["nickname"] = res->nickname;
                result["avatar_url"] = res->avatar_url;
                result["user_id"] = (Json::Value::UInt64)res->id;
            } else {
                result["status"] = "fail";
            }
        });
        server.RegistryMethod(factory.build());
    }

    // 3. Token 校验接口: UserVerifyToken (新增)
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("UserVerifyToken");
        factory.SetParamsDesc("token", VType::STRING);
        factory.SetReturnVtype(VType::OBJECT);
        factory.SetServiceDescribe([&](const Json::Value &params, Json::Value &result) {
            std::string t = params["token"].asString();
            auto res = user_manager.VerifyToken(t);
            if (res) {
                result["status"] = "success";
                result["username"] = res->username;
                result["nickname"] = res->nickname;
                result["avatar_url"] = res->avatar_url;
                result["user_id"] = (Json::Value::UInt64)res->id;
            } else {
                result["status"] = "fail";
            }
        });
        server.RegistryMethod(factory.build());
    }

    // 4. 更新资料接口: UpdateUserProfile
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("UpdateUserProfile");
        factory.SetParamsDesc("user_id", VType::INTEGRAL);
        factory.SetParamsDesc("nickname", VType::STRING);
        factory.SetParamsDesc("image_data", VType::STRING);
        factory.SetReturnVtype(VType::BOOL);
        factory.SetServiceDescribe([&](const Json::Value &params, Json::Value &result) {
            uint64_t user_id = params["user_id"].asUInt64();
            std::string nickname = params["nickname"].asString();
            std::string image_data = params["image_data"].asString();

            std::string temp_path = "";
            if (!image_data.empty()) {
                temp_path = "./temp_profile_" + std::to_string(user_id) + "_" +
                            std::to_string(time(NULL)) + ".jpg";
                std::ofstream outfile(temp_path, std::ios::binary);
                if (outfile) {
                    outfile.write(image_data.data(), image_data.size());
                    outfile.close();
                }
            }
            result = user_manager.UpdateProfile(user_id, nickname, temp_path);
        });
        server.RegistryMethod(factory.build());
    }

    std::cout << "User Server (Full) started on 8088..." << std::endl;
    server.Start();

    return 0;
}