# 论坛系统

#### 介绍

软件采用`Spring Boot`框架和`Mybatis`技术,搭建了一个可以发送帖子到专区的论坛,论坛支持用户私信和帖子点赞和帖子`MarkDown`编辑,可以通过访问 http://139.9.136.93:58080/sign-in.html 来使用该网站

#### 软件架构
└─java

    └─org

        └─example

            └─forum
                ├─common
                ├─config
                ├─controller
                ├─dao
                ├─exception
                ├─LogInterceptor
                ├─model
                ├─services
                │   └─impl
                └─utils

└─resource

    ├─mapper
    │  └─extension
    ├─mybatis

![输入图片说明](image2.png)

#### 安装教程

1.  使用`idea`的`Maven`的`package`将项目打包成`.jar`文件

2.  将`.jar`文件放入服务器的目录`/gch/gch_forum`当中,直接使用`rz -E`传输即可
3.  再将数据库代码放入该目录下，同样使用`rz -E` 
4.  确保服务器安装了`MySQL5.3`的版本,并且进入数据库管理,`mysql -uroot -p`
5.  使用`source /gch/gch_forum/forum_db`在`MySQL`中创建我们需要的数据库资源
6.  在`/log`中创建一个`forum`文件,用于保存软件的执行日志
7.  运行`.jar`程序,`nohup java -jar forum.jar &`,在程序后台运行
8.  修改 `root` 认证方式为密码登录
9.  `ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY` '密码';
10. `FLUSH PRIVILEGES`;

#### 使用说明
待开发...


#### 特技

待开发....