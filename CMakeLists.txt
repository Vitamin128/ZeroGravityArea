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

# 定义库（这里先按 INTERFACE 处理，如果你有 cpp 文件可以后续添加）
add_library(gchrpc INTERFACE)

# 设置包含路径
target_include_directories(gchrpc INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
    $<INSTALL_INTERFACE:include/gchrpc>
    ${MUDUO_INCLUDE_DIR}
)

# 链接 muduo (对于接口库用 INTERFACE 链接)
target_link_libraries(gchrpc INTERFACE ${MUDUO_LIBS})

# 安装指令
install(DIRECTORY src/include/
        DESTINATION include/gchrpc
        FILES_MATCHING PATTERN "*.hpp"
)
