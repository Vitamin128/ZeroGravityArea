#!/bin/bash
# 1. 自动化构建 C++ 项目
echo "--- 正在构建 C++ 项目 ---"
mkdir -p build && cd build
cmake ..

# --- 移除 sudo 和密码，适配 Docker root 环境 ---
echo "--- 正在安装头文件到系统路径 ---"
make install/fast

# --- 然后再编译测试程序 ---
echo "--- 正在编译测试程序 ---"
make -j4
cd ..

# 2. 启动 Python PDF 解析服务
echo "--- 正在重启 Python PDF 解析服务 ---"
pkill -f pdf_parser.py 2>/dev/null
nohup python3 ./src/pdf_parser.py > ./pdf_parser.log 2>&1 &

echo "--- 构建与启动完成！ ---"
