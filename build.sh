#!/bin/bash

# 1. 进入 build 目录并安装最新的头文件到系统目录
echo "--- 正在安装最新的头文件 ---"
cd /home/bamboo/ZeroGravityArea/build
sudo make install

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
