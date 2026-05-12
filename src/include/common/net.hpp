// #include<muduo/base/Logging.h>
#pragma once
#include <muduo/net/Buffer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/EventLoopThread.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpClient.h>
#include <muduo/net/TcpConnection.h>
#include <muduo/net/TcpServer.h>

#include <mutex>
#include <unordered_map>

#include "abstract.hpp"
#include "fields.hpp"
#include "logger.hpp"
#include "message.hpp"

namespace gchrpc {
class MuduoBuffer : public BaseBuffer {
public:
    using ptr = std::shared_ptr<MuduoBuffer>;
    MuduoBuffer(muduo::net::Buffer *buf) {
        _buf = buf;
    }
    // 查看缓冲区中还有多少字节可读
    virtual size_t readableSize() override {
        return _buf->readableBytes();
    }

    // 返回前4个字节的整型值但不读取
    virtual int32_t peekInt32() override {
        return _buf->peekInt32();
    }

    // 丢弃前面四个字节的数据,配合peekInt32使用读取
    virtual void retrieveInt32() override {
        return _buf->retrieveInt32();
    }

    // 读取前面4个字节的数据并移动读取指针
    virtual int32_t readInt32() override {
        return _buf->readInt32();
    }

    // 读取前len个字节的数据,并移动指针,以string的形式返回
    virtual std::string retrieveAsString(size_t len) override {
        return _buf->retrieveAsString(len);
    }

private:
    muduo::net::Buffer *_buf;
};

class BufferFactory {
public:
    // Args模板用于打包参数
    template <typename... Args>
    static MuduoBuffer::ptr create(Args &&...args) {
        // forward<Args>,其中Args用于args的左值右值转化的参照...用于拆解args和Args
        return std::make_shared<MuduoBuffer>(std::forward<Args>(args)...);
    }
};

class LVProtocol : public BaseProtocol {
public:
    using ptr = std::shared_ptr<LVProtocol>;
    virtual bool canProcessed(const BaseBuffer::ptr &buf) override {
        // 检查缓冲区中是否有4个字节,如果4个字节都没有那连报文都不知道多长
        if (buf->readableSize() < lenFieldsLength) {
            return false;
        }
        // 将前4个字节表示的报文长度返回给total_len,表示这个报文主体的大小
        int32_t total_len = buf->peekInt32();

        // 不仅报文的主体,还需要前面表示大小的前4个字节
        if (buf->readableSize() < (total_len + lenFieldsLength)) {
            return false;
        }
        return true;
    }
    virtual bool onMessage(const BaseBuffer::ptr &buf, BaseMessage::ptr &msg) override {
        int total_len = buf->readInt32();       // 获取报文总长度
        MType mtype = (MType)buf->readInt32();  // 获取消息类型
        int32_t idlen = buf->readInt32();       // 获取消息id的长度
        int32_t body_len =
            total_len - idlen - idlenFieldsLength - mtypeFieldsLength;  // 获取消息主体的长度
        std::string id = buf->retrieveAsString(idlen);                  // 获取消息id
        std::string body = buf->retrieveAsString(body_len);             // 获取消息主体
        msg = MessageFactory::create(mtype);                            // 创建一个消息对象
        if (!msg) {
            LOG(ERROR) << "消息类型错误，构造消息对象失败" << std::endl;
            return false;
        }
        bool ret = msg->unserialize(body);
        if (!ret) {
            LOG(ERROR) << "消息正文反序列化失败" << std::endl;
            return false;
        }
        msg->setId(id);        // 设置消息id
        msg->setMType(mtype);  // 设置消息类型,虽然这个类已经是消息类型,但还是要明确一下的
        return true;
    }

    virtual std::string serialize(const BaseMessage::ptr &msg) override {
        std::string body = msg->serialize();           // 主体消息的大小
        std::string id = msg->rid();                   // 取出id
        int32_t mtype = htonl((int32_t)msg->mtype());  // 将mtype转化为网络字节序
        int32_t alllen =
            idlenFieldsLength + mtypeFieldsLength + id.size() + body.size();  // 计算全部的大小
        int32_t nall = htonl(alllen);  // 将全部的大小转化为网络字节序

        int32_t idlen = htonl(id.size());  // 将id长度的大小计算并转化为网络字节序
        std::string result;
        result.reserve(alllen + lenFieldsLength);
        result.append((char *)&nall, lenFieldsLength);     // 第一个加全部的大小
        result.append((char *)&mtype, mtypeFieldsLength);  // 第二个家mtype的大小
        result.append((char *)&idlen, idlenFieldsLength);  // 第三个加id的大小
        result.append(id);  // string无需转化为网络字节序,因为字符只占一个字节
        result.append(body);
        return result;
    }

private:
    // 消息头部,存储了整个报文的总长度值
    const size_t lenFieldsLength = 4;
    // 消息类型放在第二位
    const size_t mtypeFieldsLength = 4;
    // 消息ID的长度
    const size_t idlenFieldsLength = 4;
    // 只有消息类型和消息ID是不在body中的所以要放到外面来
};

class Protocolfactory {
public:
    // 包模板
    template <typename... Args>
    static LVProtocol::ptr create(Args &&...args)  // 折叠引用,让左值保持左值,右值保持右值
    {
        return std::make_shared<LVProtocol>(std::forward<Args>(args)...);
    }
};

class MuduoConnection : public BaseConnection {
public:
    using ptr = std::shared_ptr<MuduoConnection>;

    // TcpConnectionPtr是用来控制一个端口和另一个端口的连接,传入一个协议一个连接即可,每个客户端只要一个协议即可
    MuduoConnection(const muduo::net::TcpConnectionPtr &conn, const BaseProtocol::ptr &protocol)
        : _protocol(protocol), _conn(conn) {}
    virtual void send(const BaseMessage::ptr &msg) override {
        // 将BaseMessage的对象转化成string
        std::string body = _protocol->serialize(msg);
        // 用指针控制的连接发送数据
        _conn->send(body);
    }
    virtual void shutdown() override {
        // 关闭连接
        _conn->shutdown();
    }
    virtual bool connected() override {
        // 检查连接是否进行
        return _conn->connected();
    }
    virtual Address peerAddress() override {
        return std::make_pair(_conn->peerAddress().toIp(), _conn->peerAddress().port());
    }

private:
    BaseProtocol::ptr _protocol;         // 连接的协议
    muduo::net::TcpConnectionPtr _conn;  // muduo库中的连接
};

class Connectionfactory {
public:
    template <typename... Args>
    static MuduoConnection::ptr create(Args &&...args) {
        return std::make_shared<MuduoConnection>(std::forward<Args>(args)...);
    }
};

// Server服务端
class MuduoServer : public BaseServer {
public:
    using ptr = std::shared_ptr<MuduoServer>;

    // 初始化_server,设置端口
    MuduoServer(int port)
        : _protocol(Protocolfactory::create()),
          _server(&_baseloop, muduo::net::InetAddress("0.0.0.0", port), "MuduoServer",
                  muduo::net::TcpServer::
                      kReusePort)  // kReusePort表示端口复用,可以多个进程绑定同一个端口
    {
        _server.setConnectionCallback(
            std::bind(&MuduoServer::onConnection, this, std::placeholders::_1));
        _server.setMessageCallback(
            std::bind(&MuduoServer::onMessage, this, std::placeholders::_1, std::placeholders::_2,
                      std::placeholders::_3));  // bind函数将函数和参数绑定起来,使得函数可以接受参数
    }
    virtual void start() override {
        _server.setThreadNum(4);
        _server.start();  // 启动服务
        // _server.setThreadNum(4);
        _baseloop.loop();  // 开始监听套接字
    }

    virtual void broadcast(const BaseMessage::ptr &msg) override {
        std::unique_lock<std::mutex> lock(_mutex);
        for (auto &pair : _conns) {
            pair.second->send(msg);
        }
    }

    virtual void runEvery(double interval_s, std::function<void()> cb) override {
        _baseloop.runEvery(interval_s, cb);
    }

private:
    void onConnection(const muduo::net::TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG(NORMAL) << "MuduoServer: 新连接建立" << std::endl;
            auto muduo_conn = Connectionfactory::create(conn, _protocol);
            {
                std::unique_lock<std::mutex> lock(_mutex);
                _conns.insert(std::make_pair(conn, muduo_conn));
            }
            if (_cb_connection) _cb_connection(muduo_conn);
        } else {
            LOG(NORMAL) << "MuduoServer: 连接断开" << std::endl;
            BaseConnection::ptr muduo_conn;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _conns.find(conn);
                if (it == _conns.end()) {
                    return;
                }
                muduo_conn = it->second;
                _conns.erase(it->first);
            }
            if (_cb_close) _cb_close(muduo_conn);
        }
    }
    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                   muduo::Timestamp) {
        LOG(NORMAL) << "MuduoServer: 收到数据，开始处理" << std::endl;
        auto base_buf = BufferFactory::create(buf);
        while (1) {
            if (_protocol->canProcessed(base_buf) == false) {
                if (base_buf->readableSize() > maxDataSize) {
                    conn->shutdown();
                    LOG(ERROR) << "MuduoServer: 缓冲区数据超过最大限制，关闭连接" << std::endl;
                    return;
                }
                break;
            }
            BaseMessage::ptr msg;
            bool ret = _protocol->onMessage(base_buf, msg);
            if (ret == false) {
                conn->shutdown();
                LOG(ERROR) << "MuduoServer: 报文解析失败，关闭连接" << std::endl;
                return;
            }
            BaseConnection::ptr base_conn;
            {
                std::unique_lock<std::mutex> lock(_mutex);
                auto it = _conns.find(conn);
                if (it == _conns.end()) {
                    conn->shutdown();
                    return;
                }
                base_conn = it->second;
            }
            if (_cb_message) {
                _cb_message(base_conn, msg);
            }
        }
    }

private:
    const size_t maxDataSize = (1 << 16);
    BaseProtocol::ptr _protocol;
    // 用于监听设置在服务端中的套接字包括连接和消息监听的
    muduo::net::EventLoop _baseloop;
    // 用于建立服务的,内部封装listen和bind和accept,并且设置callback函数
    muduo::net::TcpServer _server;
    // 锁住对_conns资源的访问操作
    std::mutex _mutex;
    // BaseConnection里面的TcpConnectionPtr和BaseMessage::ptr的映射关系
    std::unordered_map<muduo::net::TcpConnectionPtr, BaseConnection::ptr> _conns;
};

class Serverfactory {
public:
    template <typename... Args>
    static MuduoServer::ptr create(Args &&...args) {
        return std::make_shared<MuduoServer>(std::forward<Args>(args)...);
    }
};

// Client客户端
class MuduoClient : public BaseClient {
public:
    using ptr = std::shared_ptr<MuduoClient>;
    // 初始化_client,设置ip和端口
    MuduoClient(const std::string &sip, int sport)
        : _protocol(Protocolfactory::create()),
          // 用户客户端等待连接
          _downlatch(1),
          // 创建一个线程,用于监听套接字
          _baseloop(_loopthread.startLoop()),
          _client(_baseloop, muduo::net::InetAddress(sip, sport), "MuduoClient") {}

    virtual void connect() override {
        _client.setConnectionCallback(
            std::bind(&MuduoClient::onConnection, this, std::placeholders::_1));
        _client.setMessageCallback(std::bind(&MuduoClient::onMessage, this, std::placeholders::_1,
                                             std::placeholders::_2, std::placeholders::_3));
        _client.connect();
        _downlatch.wait();
        LOG(NORMAL) << "MuduoClient: 连接服务端完成" << std::endl;
    }

    virtual void shutdown() override {
        return _client.disconnect();  // 断开连接
    }
    // 发送Message给服务端
    virtual bool send(const BaseMessage::ptr &msg) override {
        BaseConnection::ptr conn;
        {
            std::unique_lock<std::mutex> lock(_mutex);
            if (!_conn || !_conn->connected()) {
                LOG(ERROR) << "MuduoClient: 连接已断开，无法发送消息" << std::endl;
                return false;
            }
            conn = _conn;
        }
        conn->send(msg);
        return true;
    }

    // 返回和服务端的BaseConnection对象
    virtual BaseConnection::ptr connection() override {
        std::unique_lock<std::mutex> lock(_mutex);
        return _conn;
    }

    // 检查与服务端的连接
    virtual bool connected() override {
        std::unique_lock<std::mutex> lock(_mutex);
        return (_conn && _conn->connected());
    }

private:
    // 连接回调函数如果建立连接则让_client停止阻塞
    void onConnection(const muduo::net::TcpConnectionPtr &conn) {
        if (conn->connected()) {
            LOG(NORMAL) << "MuduoClient: 连接建立" << std::endl;
            std::unique_lock<std::mutex> lock(_mutex);
            _conn = Connectionfactory::create(conn, _protocol);
        } else {
            LOG(NORMAL) << "MuduoClient: 连接断开" << std::endl;
            std::unique_lock<std::mutex> lock(_mutex);
            _conn.reset();
        }
        _downlatch.countDown();
    }

    // 将数据写入到缓冲区当中
    void onMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buf,
                   muduo::Timestamp) {
        LOG(NORMAL) << "MuduoClient: 收到数据，开始处理" << std::endl;
        MuduoBuffer::ptr base_buf = BufferFactory::create(buf);
        while (1) {
            if (_protocol->canProcessed(base_buf) == false) {
                if (buf->readableBytes() > _maxBufferSize) {
                    conn->shutdown();
                    LOG(ERROR) << "MuduoClient: 可读字节超过限制，关闭连接" << std::endl;
                    return;
                }
                break;
            }
            BaseMessage::ptr msg;
            bool ret = _protocol->onMessage(base_buf, msg);
            if (ret == false) {
                conn->shutdown();
                LOG(ERROR) << "MuduoClient: 报文解析失败，关闭连接" << std::endl;
                return;
            }

            // 处理回调
            if (_cb_message) {
                _cb_message(_conn, msg);
            }
        }
    }

private:
    static const int _maxBufferSize = (1 << 31);
    BaseProtocol::ptr _protocol;              // 协议,用于将缓冲区中的数据转化为BaseMessage
    BaseConnection::ptr _conn;                // 和服务端的连接
    std::mutex _mutex;                        // 保护 _conn 的并发访问
    muduo::CountDownLatch _downlatch;         // 用于等待连接建立
    muduo::net::EventLoopThread _loopthread;  // 用于创建新的线程监听套接字
    muduo::net::EventLoop *_baseloop;         // 存了子线程创建的muduo::net::EventLoop
    muduo::net::TcpClient _client;            // 用于建立连接的客户端
};

class Clientfactory {
public:
    template <typename... Args>
    static MuduoClient::ptr create(Args &&...args) {
        return std::make_shared<MuduoClient>(std::forward<Args>(args)...);
    }
};
}  // namespace gchrpc