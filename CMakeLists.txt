cmake_minimum_required(VERSION 3.10)
project(gchrpc)

# 设置编译器
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# muduo 路径设置
set(MUDUO_INCLUDE_DIR "/usr/local/include")
set(MUDUO_LIBS 
    "/usr/local/lib/libmuduo_net.a" 
    "/usr/local/lib/libmuduo_base.a"
)

# 定义 gchrpc 为接口库（Header-only 框架）
add_library(gchrpc INTERFACE)

# 设置包含路径
target_include_directories(gchrpc INTERFACE 
    # 开发阶段：相对于 src/include 查找头文件
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
    # 安装阶段：用户可以通过 <gchrpc/xxx/yyy.hpp> 引用
    $<INSTALL_INTERFACE:include>
    ${MUDUO_INCLUDE_DIR}
)

# 链接依赖
target_link_libraries(gchrpc INTERFACE ${MUDUO_LIBS})

# 安装指令：将头文件安装到 include/gchrpc 目录下
install(DIRECTORY src/include/
        DESTINATION include/gchrpc
        FILES_MATCHING PATTERN "*.hpp"
)
