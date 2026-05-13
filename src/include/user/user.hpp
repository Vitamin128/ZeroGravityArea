#pragma once

#include <iomanip>
#include <iostream>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <cstdio>

// OpenSSL EVP
#include <openssl/evp.h>

// SOCI
#include <soci/mysql/soci-mysql.h>
#include <soci/soci.h>

namespace user {

// ============================================================
// 数据库连接信息
// ============================================================
static const std::string DB_CONNECT_STR =
    "host=127.0.0.1 port=3306 user=root password=041215 dbname=AreaData";

// ============================================================
// 对应数据库 users 表的实体结构
// ============================================================
struct UserRecord {
    uint64_t id;
    std::string username;
    std::string password_hash;
    std::string salt;
    std::string nickname;
    std::string avatar_url;
    uint32_t login_count;
    std::string last_login_at;
    std::string created_at;
    std::string current_token;    // 当前有效 Token
    std::string token_expire_at;  // 过期时间
};

// ============================================================
// 用户管理类
// ============================================================
class UserManager {
public:
    /**
     * @brief 用户注册
     */
    bool Register(const std::string &username, const std::string &password, const std::string &nickname) {
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);
            int count = 0;
            sql << "SELECT COUNT(*) FROM users WHERE username = :u", soci::into(count), soci::use(username);
            if (count > 0) return false;

            std::string salt = GenerateSalt();
            std::string passwd_hash = ComputeHash(password, salt);

            sql << "INSERT INTO users (username, password_hash, salt, nickname, created_at) "
                   "VALUES (:u, :h, :s, :n, NOW())",
                soci::use(username), soci::use(passwd_hash), soci::use(salt), soci::use(nickname);
            return true;
        } catch (const soci::soci_error &e) {
            std::cerr << "Register Error: " << e.what() << std::endl;
            return false;
        }
    }

    /**
     * @brief 用户登录
     */
    std::optional<UserRecord> Login(const std::string &username, const std::string &password) {
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);
            UserRecord record;
            sql << "SELECT id, username, password_hash, salt, "
                   "COALESCE(nickname,''), COALESCE(avatar_url,''), "
                   "COALESCE(login_count,0), "
                   "COALESCE(DATE_FORMAT(last_login_at,'%Y-%m-%d %H:%i:%s'),''), "
                   "COALESCE(DATE_FORMAT(created_at,'%Y-%m-%d %H:%i:%s'),''), "
                   "COALESCE(current_token,''), "
                   "COALESCE(DATE_FORMAT(token_expire_at,'%Y-%m-%d %H:%i:%s'),'') "
                   "FROM users WHERE username = :u LIMIT 1",
                soci::into(record.id), soci::into(record.username),
                soci::into(record.password_hash), soci::into(record.salt),
                soci::into(record.nickname), soci::into(record.avatar_url),
                soci::into(record.login_count), soci::into(record.last_login_at),
                soci::into(record.created_at), soci::into(record.current_token),
                soci::into(record.token_expire_at), soci::use(username);

            if (!sql.got_data()) return std::nullopt;

            if (ComputeHash(password, record.salt) != record.password_hash) return std::nullopt;

            std::string new_token = GenerateToken();
            sql << "UPDATE users SET login_count = login_count + 1, last_login_at = NOW(), "
                   "current_token = :t, token_expire_at = DATE_ADD(NOW(), INTERVAL 30 DAY) "
                   "WHERE id = :id",
                soci::use(new_token), soci::use(record.id);

            record.current_token = new_token;
            return record;
        } catch (const soci::soci_error &e) {
            std::cerr << "Login Error: " << e.what() << std::endl;
            return std::nullopt;
        }
    }

    /**
     * @brief 验证 Token
     */
    std::optional<UserRecord> VerifyToken(const std::string &token) {
        if (token.empty()) return std::nullopt;
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);
            UserRecord record;
            sql << "SELECT id, username, nickname, avatar_url, login_count "
                   "FROM users WHERE current_token = :t AND token_expire_at > NOW() LIMIT 1",
                soci::into(record.id), soci::into(record.username),
                soci::into(record.nickname), soci::into(record.avatar_url),
                soci::into(record.login_count), soci::use(token);

            if (sql.got_data()) return record;
        } catch (const soci::soci_error &e) {
            std::cerr << "VerifyToken Error: " << e.what() << std::endl;
        }
        return std::nullopt;
    }

    /**
     * @brief 上传头像到腾讯云 COS (通过调用 Python 脚本)
     */
    std::string UploadAvatar(const std::string &local_path) {
        // 命令：python3 src/cos_uploader.py [路径]
        std::string cmd = "python3 src/cos_uploader.py " + local_path;
        
        FILE* pipe = popen(cmd.c_str(), "r");
        if (!pipe) return "";

        char buffer[256];
        std::string result = "";
        while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            result += buffer;
        }
        
        int status = pclose(pipe);
        if (status == 0) {
            if (!result.empty() && result.back() == '\n') result.pop_back();
            return result;
        }
        return "";
    }

private:
    std::string ComputeHash(const std::string &password, const std::string &salt) {
        std::string data = password + salt;
        unsigned char hash[64]; 
        unsigned int len = 0;
        EVP_MD_CTX* ctx = EVP_MD_CTX_new();
        EVP_DigestInit_ex(ctx, EVP_sha256(), NULL);
        EVP_DigestUpdate(ctx, data.c_str(), data.size());
        EVP_DigestFinal_ex(ctx, hash, &len);
        EVP_MD_CTX_free(ctx);

        std::stringstream ss;
        for (unsigned int i = 0; i < len; i++) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    std::string GenerateRandomString(size_t len) {
        static const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        static std::mt19937 rg{std::random_device{}()};
        static std::uniform_int_distribution<std::string::size_type> pick(0, sizeof(charset) - 2);
        std::string s;
        s.reserve(len);
        for (size_t i = 0; i < len; ++i) s += charset[pick(rg)];
        return s;
    }

    std::string GenerateSalt() { return GenerateRandomString(16); }
    std::string GenerateToken() { return GenerateRandomString(64); }
};

}  // namespace user