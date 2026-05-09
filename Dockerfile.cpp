# 使用 Ubuntu 作为基础镜像
FROM ubuntu:20.04

# 避免交互式确认
ENV DEBIAN_FRONTEND=noninteractive

# 安装运行所需的库 (g++ 用于运行时的 libstdc++, boost, jsoncpp 等)
RUN apt-get update && apt-get install -y \
    libboost-system-dev \
    libboost-filesystem-dev \
    libjsoncpp-dev \
    libssl-dev \
    libcrypto++-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /app

# 拷贝预编译好的二进制文件 (假设已经在宿主机 build 好了)
# 或者你可以选择在容器内编译，但那样镜像会很大且慢
COPY build/server /app/server
COPY build/http_server /app/http_server

# 拷贝必要的词库文件 (cppjieba 需要)
COPY /usr/local/share/cppjieba/dict /usr/local/share/cppjieba/dict

# 默认不启动，由 docker-compose 指定启动哪个程序
CMD ["./server"]
