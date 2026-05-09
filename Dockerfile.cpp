# --- 第一阶段：编译阶段 (Builder Stage) ---
FROM ubuntu:20.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

# 安装编译所需的最小化工具链 (换源以加速)
RUN sed -i 's/archive.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libjsoncpp-dev \
    libssl-dev \
    libcrypto++-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /build_tmp
# 拷贝整个项目（包含 depends 文件夹）
COPY . .

# 执行编译
RUN mkdir -p build && cd build && cmake .. && make -j4

# --- 第二阶段：运行阶段 (Runtime Stage) ---
FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

# 安装运行所需的动态库
RUN apt-get update && apt-get install -y \
    libjsoncpp-dev \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

# 从 builder 阶段提取二进制文件
COPY --from=builder /build_tmp/build/server /app/server
COPY --from=builder /build_tmp/build/http_server /app/http_server

# 拷贝本地携带的词库到容器系统目录（保持代码中宏定义的路径有效）
COPY --from=builder /build_tmp/depends/share/dict /usr/local/share/cppjieba/dict

# 默认启动后端服务器
CMD ["./server"]
