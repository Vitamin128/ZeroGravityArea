#!/bin/bash

# 1. 将 src/include 下的文件软链接到系统目录 /usr/local/include/gchrpc
echo "--- 正在建立头文件软链接 ---"
# 如果目标已存在且不是软链接，先删除（备份）以防冲突
if [ -d "/usr/local/include/gchrpc" ] && [ ! -L "/usr/local/include/gchrpc" ]; then
    sudo mv /usr/local/include/gchrpc /usr/local/include/gchrpc.bak
fi
sudo ln -sf /home/bamboo/ZeroGravityArea/src/include /usr/local/include/gchrpc
echo "软链接已建立: /usr/local/include/gchrpc -> /home/bamboo/ZeroGravityArea/src/include"

# 2. 进入测试目录并编译
echo "--- 正在编译 server 和 client ---"
cd /home/bamboo/ZeroGravityArea/test

# 编译服务端
g++ -std=c++17 /home/bamboo/ZeroGravityArea/test/server.cc -o server \
    -I/usr/local/include \
    -I/home/bamboo/ZeroGravityArea/cppjieba/include \
    -I/home/bamboo/ZeroGravityArea/cppjieba/deps/limonp/include \
    -ljsoncpp -lmuduo_net -lmuduo_base -lpthread \
    -lboost_filesystem -lboost_system \
    -lcpp-httplib -lssl -lcrypto

# 编译客户端
g++ -std=c++17 /home/bamboo/ZeroGravityArea/test/client.cc -o client \
    -I/usr/local/include \
    -ljsoncpp -lmuduo_net -lmuduo_base -lpthread

# 编译 HTTP 桥接服务端 (用于连接 Web 前端)
g++ -std=c++17 /home/bamboo/ZeroGravityArea/test/http_server.cc -o http_server \
    -I/usr/local/include \
    -ljsoncpp -lmuduo_net -lmuduo_base -lpthread \
    -lcpp-httplib -lssl -lcrypto

echo "--- 编译完成！ ---"

# 3. 启动 Python PDF 解析服务 (后台运行)
echo "--- 正在重启 Python PDF 解析服务 ---"
# 先尝试关闭旧的解析服务，防止端口冲突
pkill -f pdf_parser.py 2>/dev/null

# 使用 nohup 后台启动，注入全套 UTF-8 环境变量
nohup env PYTHONIOENCODING=utf-8 \
          LANG=C.UTF-8 \
          LC_ALL=C.UTF-8 \
          LANGUAGE=C.UTF-8 \
          python3 /home/bamboo/ZeroGravityArea/src/pdf_parser.py > /home/bamboo/ZeroGravityArea/pdf_parser.log 2>&1 &

echo "--- Python 服务已在后台启动 (端口 8080) ---"
