#!/bin/bash
# 1. 自动化构建 C++ 项目
echo "--- 正在构建 C++ 项目 ---"
mkdir -p build && cd build
cmake ..
make -j4
# (可选) 执行安装
echo 041215 | sudo -S make install
# 2. 启动 Python PDF 解析服务 (原有逻辑)
echo "--- 正在重启 Python PDF 解析服务 ---"
pkill -f pdf_parser.py 2>/dev/null
nohup python3 /home/bamboo/ZeroGravityArea/src/pdf_parser.py > /home/bamboo/ZeroGravityArea/pdf_parser.log 2>&1 &
echo "--- 构建与启动完成！ ---"