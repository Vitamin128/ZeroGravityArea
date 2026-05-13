#include <common/message.hpp>
#include <common/net.hpp>
#include <filesystem>
#include <iostream>
#include <server/rpc_server.hpp>
#include <user/user.hpp>
#include <fstream>
#include <jsoncpp/json/json.h>

using namespace gchrpc::server;

int main() {
    // 监听 8088 端口，并向 9090 的注册中心注册
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
        factory.SetServiceDescribe([&](const Json::Value& params, Json::Value& result) {
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
        factory.SetServiceDescribe([&](const Json::Value& params, Json::Value& result) {
            std::string u = params["username"].asString();
            std::string p = params["password"].asString();
            auto res = user_manager.Login(u, p);
            if (res) {
                result["status"] = "success";
                result["token"] = res->current_token;
                result["nickname"] = res->nickname;
                result["avatar_url"] = res->avatar_url;
            } else {
                result["status"] = "fail";
            }
        });
        server.RegistryMethod(factory.build());
    }

    // 3. 上传头像接口: UploadAvatar
    {
        ServiceDescribeFactory factory;
        factory.SetMethodName("UploadAvatar");
        factory.SetParamsDesc("user_id", VType::STRING);
        factory.SetParamsDesc("image_data", VType::STRING); // 二进制数据以字符串形式传输
        factory.SetReturnVtype(VType::STRING);
        factory.SetServiceDescribe([&](const Json::Value& params, Json::Value& result) {
            std::string user_id = params["user_id"].asString();
            std::string image_data = params["image_data"].asString();

            // A. 本地落盘
            std::string temp_path = "./temp_avatar_" + user_id + "_" + std::to_string(time(NULL)) + ".jpg";
            std::ofstream outfile(temp_path, std::ios::binary);
            if (!outfile) {
                result = "";
                return;
            }
            outfile.write(image_data.data(), image_data.size());
            outfile.close();

            // B. 调用 COS 上传 (会自动删除本地文件)
            std::string cloud_url = user_manager.UploadAvatar(temp_path);
            result = cloud_url;
        });
        server.RegistryMethod(factory.build());
    }

    std::cout << "User Server started on 8088 using ServiceDescribeFactory..." << std::endl;
    server.Start();
    
    return 0;
}