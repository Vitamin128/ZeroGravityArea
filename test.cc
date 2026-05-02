#include <boost/asio.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <iostream>
#include <string>
#include <vector>

// 示例 0: 使用 boost::asio::streambuf
// 它是最传统的 Asio 动态缓冲区，基于 std::streambuf
void func0() {
    std::cout << "--- func0: boost::asio::streambuf ---" << std::endl;
    boost::asio::streambuf buf;

    // 1. 写入数据: 使用 std::ostream 像操作文件一样写入
    std::ostream os(&buf);
    os << "Hello Asio Streambuf";

    // 2. 查看当前大小
    std::cout << "Buffer size: " << buf.size() << std::endl;

    // 3. 读取数据: 使用 std::istream 读取
    std::istream is(&buf);
    std::string result;
    std::getline(is, result);

    std::cout << "Read result: " << result << std::endl;
    std::cout << "Size after read: " << buf.size() << std::endl;  // 读取后会自动消耗（consume）
}

// 示例 1: 使用 boost::asio::dynamic_buffer
// 它将现有的容器（如 std::string 或 std::vector）包装成异步 IO 可用的缓冲区
void func1() {
    std::cout << "\n--- func1: boost::asio::dynamic_buffer ---" << std::endl;
    std::string data;
    auto buf = boost::asio::dynamic_buffer(data);

    // 1. 准备空间并写入 (通常在 async_read 中自动发生，这里手动演示)
    const std::string msg = "Dynamic Buffer Data";
    // 模拟写入：扩展底层容器并拷贝数据
    size_t n = buf.prepare(msg.size()).size();
    std::copy(msg.begin(), msg.end(), std::back_inserter(data));
    buf.commit(msg.size());  // 提交数据使其变为“可读”

    // 2. 查看底层容器
    std::cout << "Underlying string: " << data << std::endl;
    std::cout << "Buffer size: " << buf.size() << std::endl;

    // 3. 消耗数据: 移动读取指针（即删除字符串前面的数据）
    buf.consume(8);  // 删掉 "Dynamic "
    std::cout << "Remaining data: " << data << std::endl;
}

// 示例 2: 使用 boost::beast::flat_buffer
// 这是 Beast 库提供的，最像 muduo::Buffer 的一个，因为它保证内存是连续的
void func2() {
    std::cout << "\n--- func2: boost::beast::flat_buffer ---" << std::endl;
    boost::beast::flat_buffer buf;

    // 1. 获取可写空间
    auto writable = buf.prepare(32);
    std::string msg = "Beast Flat Buffer Content";
    std::memcpy(writable.data(), msg.data(), msg.size());

    // 2. 提交数据
    buf.commit(msg.size());

    // 3. 读取数据 (使用 data() 获取连续内存指针)
    auto readable = buf.data();
    std::string result(static_cast<const char *>(readable.data()), readable.size());
    std::cout << "Read result: " << result << std::endl;

    // 4. 清理已读部分
    buf.consume(6);  // 消耗 "Beast "
    std::cout << "Size after consume: " << buf.size() << std::endl;
}

int main() {
    func1();
    return 0;
}
