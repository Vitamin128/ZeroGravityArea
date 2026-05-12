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
    std::cout << "RPC Client initialized" << std::endl;
    // 映射文件路径：每次启动程序时扫描指定目录下的 pdf 和 html 文件
    std::unordered_map<std::string, int> file_exists_map;
    auto scan_dir = [&](const std::string &path) {
        if (!std::filesystem::exists(path)) return;
        for (const auto &entry : std::filesystem::recursive_directory_iterator(path)) {
            if (entry.is_regular_file()) {
                std::string ext = entry.path().extension().string();
                if (ext == ".html" || ext == ".pdf") {
                    file_exists_map[entry.path().string()] = 1;
                }
            }
        }
    };
    scan_dir("./InternalData/html");
    scan_dir("./InternalData/pdf");
    std::cout << "Successfully mapped " << file_exists_map.size() << " files into memory."
              << std::endl;

    // 2. 初始化 HTTP Server (cpphttplib)
    httplib::Server svr;

    svr.set_mount_point("/", "./dist");
    // 2. 配置 SPA 兜底逻辑
    // 如果用户刷新页面或直接输入 /search 这种非文件的路径，
    // 默认返回 index.html，交给前端 Vue Router 处理。
    // 5. 处理上传请求：/upload
    svr.Post("/upload", [&](const httplib::Request &req, httplib::Response &res) {
        res.set_header("Access-Control-Allow-Origin", "*");

        // 检查是否是 multipart 且包含文件
        if (req.is_multipart_form_data() && req.files.count("file") > 0) {
            auto now = std::chrono::system_clock::now();
            auto timestamp =
                std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            std::string ts_str = std::to_string(timestamp);

            std::string html_base = "./ExternalData/html/" + ts_str;
            std::string pdf_base = "./ExternalData/pdf/" + ts_str;

            // 构造传给后端的路径参数
            Json::Value paths;
            paths["html_path"] = html_base;
            paths["pdf_path"] = pdf_base;

            try {
                // 提前在循环外把两个目录都建好
                std::filesystem::create_directories(html_base);
                std::filesystem::create_directories(pdf_base);
            } catch (const std::exception &e) {
                LOG(ERROR) << "Failed to pre-create directories: " << e.what() << std::endl;
            }

            auto range = req.files.equal_range("file");
            for (auto it = range.first; it != range.second; ++it) {
                const auto &file = it->second;
                std::string filename = file.filename;
                std::string target_path = "";

                if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".html") {
                    target_path = html_base + "/" + filename;
                } else if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".pdf") {
                    target_path = pdf_base + "/" + filename;
                }

                if (!target_path.empty()) {
                    std::ofstream ofs(target_path, std::ios::binary);
                    if (ofs.is_open()) {
                        ofs.write(file.content.c_str(), file.content.size());
                        ofs.close();
                        std::cout << "✅ 快速保存: " << target_path << std::endl;
                        file_exists_map[target_path] = 1;
                    }
                }
            }

            // 【关键新增】调用后端的 IndexService 触发索引构建
            Json::Value index_resp;
            bool index_ok = rpc_client.call("IndexService", paths, index_resp);

            // 构造返回给前端的响应
            Json::Value response;
            response["status"] = index_ok ? "success" : "partial_success";
            response["message"] = index_ok ? "上传并索引完成" : "文件已保存但索引触发失败";
            response["data"] = paths;

            Json::FastWriter writer;
            res.status = 200;
            res.set_content(writer.write(response), "application/json");
            return;
        }

        // 兜底的错误处理
        res.status = 400;
        res.set_content(R"({"status": "error", "message": "上传失败，未检测到有效文件"})",
                        "application/json");
    });

    // 4. 处理下载请求：/download?doc_id=xxx
    svr.Get("/download", [&](const httplib::Request &req, httplib::Response &res) {
        if (!req.has_param("doc_id")) {
            res.set_content("Missing parameter 'doc_id'", "text/plain");
            return;
        }
        std::string doc_id_str = req.get_param_value("doc_id");
        uint64_t doc_id = std::stoull(doc_id_str);
        std::cout << "Received download request for doc_id: " << doc_id << std::endl;

        // 构造 RPC 请求
        Json::Value rpc_req;
        rpc_req["doc_id"] = (Json::UInt64)doc_id;
        Json::Value rpc_resp;

        // 调用后台 RPC Server 的 DownloadService
        if (rpc_client.call("DownloadService", rpc_req, rpc_resp)) {
            std::string path = rpc_resp.asString();  // 假设 RPC 返回的是物理路径

            // --- 核心修改：读取文件并发送 ---
            std::ifstream ifs(path, std::ios::binary);
            if (ifs.is_open()) {
                // 将文件内容读入字符串（小文件方案，大文件建议用流式发送）
                std::string content((std::istreambuf_iterator<char>(ifs)),
                                    (std::istreambuf_iterator<char>()));

                // 提取文件名
                std::string filename = std::filesystem::path(path).filename().string();
                // 设置响应头，告诉浏览器这是一个附件，需要下载
                res.set_header("Access-Control-Allow-Origin", "*");
                res.set_header("Content-Disposition", "attachment; filename=\"" + filename + "\"");
                res.set_content(content, "application/octet-stream");  // 设置为二进制流
            } else {
                res.status = 404;
                res.set_content("File not found on server", "text/plain");
            }
        } else {
            res.status = 500;
            res.set_content("RPC call failed", "text/plain");
        }
    });

    svr.Get("/search", [&](const httplib::Request &req, httplib::Response &res) {
        if (!req.has_param("word")) {
            res.set_content("Missing parameter 'word'", "text/plain");
            return;
        }
        std::string word = req.get_param_value("word");
        std::cout << "\n[HTTP_DEBUG] ======= 收到搜索请求: [" << word << "] =======" << std::endl;

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

    svr.Get(R"(/(.*))", [&](const httplib::Request &req, httplib::Response &res) {
        std::cout << "[HTTP_DEBUG] 收到未匹配请求: path=" << req.path << " query=" << req.target << std::endl;
        // 如果请求的不是 API (不以 /api 开头)，且不是已存在的静态文件
        if (req.path.find("/api") != 0) {
            std::ifstream file("./dist/index.html");
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                res.set_content(buffer.str(), "text/html");
                return;
            }
        }
        res.status = 404;
        res.set_content("Not Found", "text/plain");
    });

    std::cout << "HTTP Gateway started at http://0.0.0.0:8081" << std::endl;
    svr.listen("0.0.0.0", 8081);

    return 0;
}
