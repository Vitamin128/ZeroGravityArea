# Boost搜索引擎

## 安装教程

- 使用`make`指令构建parser、debug、server
- 执行`./parser`生成搜索文档`raw.txt`
- 执行`./server`启动服务（启动服务时会构建索引，可能需要几秒时间）
- 访问`http://localhost:8080`进行测试

## API接口

访问以下接口进行搜索：
```
http://localhost:8080/hi?query=关键词
```

- 返回JSON格式的搜索结果，包含权重、标题、描述、链接

## 项目架构

本项目主要分为三个部分：

- **预处理(parser)**：从HTML文件中提取有用信息（标题、正文、URL），并写入统一的原始数据文件`raw.txt`

- **索引构建(index)**：为搜索引擎构建正排索引和倒排索引，以便快速查找网页内容

- **搜索模块(searcher)**：在已建立的索引中查找用户输入的关键词，并将结果转换为JSON格式返回

- **Web服务端(server)**：启动后端接口以接收前端请求

## 核心特性

- **中文分词**：使用`cppjieba`进行中文分词
- **权重计算**：标题权重高于正文，提供更准确的搜索排序
- **大小写不敏感**：搜索时忽略大小写
- **文档摘要**：自动提取关键词周边内容作为摘要

## 依赖库

- **Boost**：用于文件系统操作
- **cppjieba**：用于中文分词
- **cpp-httplib**：用于HTTP服务器
- **jsoncpp**：用于JSON数据处理

## 项目结构

```
.
├── parser.cpp        # HTML解析器
├── index.hpp         # 索引模块
├── searcher.hpp      # 搜索模块
├── server.cpp        # Web服务器
└── util.hpp          # 工具类
```