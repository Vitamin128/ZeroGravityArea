cmake_minimum_required(VERSION 3.10)
project(gchrpc)

# 设置编译器
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# muduo 路径设置 (优先使用项目内自带的库，支持 Docker 跨环境编译)
# 注意：只包含 muduo/cppjieba/limonp/httplib，不包含 gchrpc（gchrpc 来自 src/include）
set(MUDUO_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/depends/include")
set(MUDUO_LIBS 
    "${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/libmuduo_net.a" 
    "${CMAKE_CURRENT_SOURCE_DIR}/depends/lib/libmuduo_base.a"
    pthread
)

# 定义 gchrpc 为接口库（Header-only 框架）
add_library(gchrpc INTERFACE)

# 注入 cppjieba 词库路径宏
add_definitions(-DCPPJIEBA_DICT_PATH="/usr/local/share/cppjieba/dict/jieba.dict.utf8")
add_definitions(-DCPPJIEBA_HMM_PATH="/usr/local/share/cppjieba/dict/hmm_model.utf8")
add_definitions(-DCPPJIEBA_USER_DICT_PATH="/usr/local/share/cppjieba/dict/user.dict.utf8")
add_definitions(-DCPPJIEBA_IDF_PATH="/usr/local/share/cppjieba/dict/idf.utf8")
add_definitions(-DCPPJIEBA_STOP_WORD_PATH="/usr/local/share/cppjieba/dict/stop_words.utf8")

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

# 1. 编译服务端
add_executable(server test/server.cc)
target_link_libraries(server gchrpc jsoncpp boost_system boost_filesystem ssl crypto)
# 2. 编译客户端
add_executable(client test/client.cc)
target_link_libraries(client gchrpc jsoncpp)
# 3. 编译 HTTP 桥接服务端
add_executable(http_server test/http_server.cc)
target_link_libraries(http_server gchrpc jsoncpp ssl crypto)