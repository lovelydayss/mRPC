## MRPC

一个高性能 RPC 框架项目


#### 开发环境
Ubuntu 18.04
xmake v2.7.9+20230515
gcc 7.5.0
vscode + ssh 远程连接开发

#### 支撑库
配置文件采用 json 实现， 此处采用 jsoncpp 进行 json 格式处理 jsoncpp

https://blog.csdn.net/Worthy_Wang/article/details/107045013
https://blog.csdn.net/shuiyixin/article/details/89330529

此外使用 protobuf 进行序列化及反序列化解析

基础网络库采用类似于 muduo 库实现

#### C++ Linux VScode 工具链配置

https://zhuanlan.zhihu.com/p/603687041

