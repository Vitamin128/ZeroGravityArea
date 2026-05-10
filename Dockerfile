# 1. 基础镜像：和开发环境保持一致
FROM ubuntu:24.04

# 2. 设置非交互环境变量，防止 apt-get install 时卡在时区/语言选择
ENV DEBIAN_FRONTEND=noninteractive

# 替换为国内镜像源 (阿里云)，并强制开启信任模式，跳过签名校验
RUN sed -i 's@//.*archive.ubuntu.com@//mirrors.aliyun.com@g' /etc/apt/sources.list.d/ubuntu.sources && \
    sed -i 's@//.*security.ubuntu.com@//mirrors.aliyun.com@g' /etc/apt/sources.list.d/ubuntu.sources && \
    sed -i 's/Types: deb/Types: deb\nTrusted: yes/' /etc/apt/sources.list.d/ubuntu.sources

# 3. 安装系统依赖 (Apt 库)
RUN apt-get update && apt-get install -y --allow-unauthenticated \
    build-essential \
    cmake \
    sudo \
    libssl-dev \
    libjsoncpp-dev \
    libboost-system-dev \
    libboost-filesystem-dev \
    libcpp-httplib-dev \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# 4. 拷贝刚刚整理好的第三方非标库 (Muduo & Jieba)
COPY ./docker_deps/include /usr/local/include/
COPY ./docker_deps/lib /usr/local/lib/
COPY ./docker_deps/share /usr/local/share/

# 5. 设置工作目录并将项目源码拷贝到容器中
WORKDIR /app
COPY . /app

# 6. 运行构建脚本（注意：build.sh 会自动执行 make install/fast 和 make）
RUN chmod +x build.sh && ./build.sh

# 7. 配置服务端启动环境变量（0.0.0.0 供外部访问）
ENV GCHRPC_HOST=0.0.0.0
ENV GCHRPC_PORT=8088

# 8. 默认启动命令 (只启动 C++ 搜索服务端，Python 解析服务建议后期分容器或单独配脚本)
CMD ["./build/server"]
