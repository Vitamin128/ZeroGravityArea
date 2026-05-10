#!/bin/bash
# 1. 自动化构建 C++ 项目
echo "--- 正在构建 C++ 项目 ---"
mkdir -p build && cd build
cmake ..

# --- 关键改动：先安装头文件，确保编译测试程序时能找到 <gchrpc/...> ---
echo "--- 正在安装头文件到系统路径 ---"
echo 041215 | sudo -S make install/fast

# --- 然后再编译测试程序 ---
echo "--- 正在编译测试程序 ---"
make -j4

# 2. 启动 Python PDF 解析服务 (原有逻辑)
echo "--- 正在重启 Python PDF 解析服务 ---"
pkill -f pdf_parser.py 2>/dev/null
nohup python3 /home/bamboo/ZeroGravityArea/src/pdf_parser.py > /home/bamboo/ZeroGravityArea/pdf_parser.log 2>&1 &

echo "--- 构建与启动完成！ ---"
