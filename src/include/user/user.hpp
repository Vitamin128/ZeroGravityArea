#ifndef USER_HPP
#define USER_HPP

#include <cstdint>
#include <iomanip>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

// OpenSSL SHA256
#include <openssl/sha.h>
// #include<mysql.h>
// SOCI
#include <soci/mysql/soci-mysql.h>
#include <soci/soci.h>

namespace user {

// ============================================================
// 数据库连接信息（对应 Docker 中的 ZeroGravityMysql 容器）
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
};

// ============================================================
// 用户管理类
// ============================================================
class UserManager {
public:
    /**
     * @brief 用户注册
     * @param username 用户名
     * @param password 明文密码
     * @return 注册成功返回 true，失败（如用户名已存在）返回 false
     */
    bool Register(const std::string &username, const std::string &password) {
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);

            // 1. 检查用户名是否已存在
            int count = 0;
            sql << "SELECT COUNT(*) FROM users WHERE username = :u", soci::into(count),
                soci::use(username);
            if (count > 0) {
                return false;  // 用户名已存在
            }

            // 2. 生成盐并计算哈希
            std::string salt = GenerateSalt();
            std::string passwd_hash = ComputeHash(password, salt);

            // 3. 插入数据库
            sql << "INSERT INTO users (username, password_hash, salt) "
                   "VALUES (:u, :h, :s)",
                soci::use(username), soci::use(passwd_hash), soci::use(salt);

            return true;
        } catch (const soci::soci_error &e) {
            // 生产环境中可以替换为日志系统
            return false;
        }
    }

    /**
     * @brief 用户登录
     * @param username 用户名
     * @param password 明文密码
     * @return 登录成功返回 UserRecord，失败返回 std::nullopt
     */
    std::optional<UserRecord> Login(const std::string &username, const std::string &password) {
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);

            // 1. 查询用户信息
            UserRecord record;
            soci::indicator nick_ind, avatar_ind, login_at_ind;

            sql << "SELECT id, username, password_hash, salt, "
                   "COALESCE(nickname,''), COALESCE(avatar_url,''), "
                   "COALESCE(login_count,0), "
                   "COALESCE(DATE_FORMAT(last_login_at,'%Y-%m-%d %H:%i:%s'),''), "
                   "DATE_FORMAT(created_at,'%Y-%m-%d %H:%i:%s') "
                   "FROM users WHERE username = :u LIMIT 1",
                soci::into(record.id), soci::into(record.username),
                soci::into(record.password_hash), soci::into(record.salt),
                soci::into(record.nickname), soci::into(record.avatar_url),
                soci::into(record.login_count), soci::into(record.last_login_at),
                soci::into(record.created_at), soci::use(username);

            if (!sql.got_data()) {
                return std::nullopt;  // 用户不存在
            }

            // 2. 验证密码：用数据库里的盐重新算哈希，比对结果
            std::string computed = ComputeHash(password, record.salt);
            if (computed != record.password_hash) {
                return std::nullopt;  // 密码错误
            }

            // 3. 登录成功：更新登录时间和次数
            sql << "UPDATE users SET login_count = login_count + 1, "
                   "last_login_at = NOW() WHERE id = :id",
                soci::use(record.id);

            return record;
        } catch (const soci::soci_error &e) {
            return std::nullopt;
        }
    }

    /**
     * @brief 更新用户信息（昵称、头像）
     * @param user_id 用户 ID
     * @param nickname 新昵称
     * @param avatar_url 新头像 URL
     * @return 更新成功返回 true
     */
    bool UpdateProfile(uint64_t user_id, const std::string &nickname,
                       const std::string &avatar_url) {
        try {
            soci::session sql(soci::mysql, DB_CONNECT_STR);

            sql << "UPDATE users SET nickname = :n, avatar_url = :a "
                   "WHERE id = :id",
                soci::use(nickname), soci::use(avatar_url), soci::use(user_id);

            return true;
        } catch (const soci::soci_error &e) {
            return false;
        }
    }

private:
    // ============================================================
    // 生成随机盐值（16位十六进制字符串）
    // ============================================================
    std::string GenerateSalt(size_t length = 16) {
        static const char hex_chars[] = "0123456789abcdef";
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dist(0, 15);

        std::string salt;
        salt.reserve(length);
        for (size_t i = 0; i < length; ++i) {
            salt += hex_chars[dist(gen)];
        }
        return salt;
    }

    // ============================================================
    // 计算 SHA256(password + salt)，返回 64 位十六进制字符串
    // ============================================================
    std::string ComputeHash(const std::string &password, const std::string &salt) {
        std::string input = password + salt;
        unsigned char digest[SHA256_DIGEST_LENGTH];

        SHA256(reinterpret_cast<const unsigned char *>(input.c_str()), input.size(), digest);

        std::ostringstream oss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(digest[i]);
        }
        return oss.str();
    }
};

}  // namespace user

#endif  // USER_HPP