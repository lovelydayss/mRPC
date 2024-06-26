# MRPC

C++11 实现的高性能协程异步 RPC 框架

## 简介

### 组件

* 轻量级非对称有栈协程组件：参考 libco 实现，基于自实现内存池进行协程模拟栈管理，基于全局单例协程池实现协程统一管理，支持协程创建、切换等常用协程操作，通过常用网络函数的 hook 劫持，采用 epoll 方法实现异步逻辑的同步式调用。
* 高性能协程异步网络库：参考 muduo 实现，采用 Reactor 事件处理模型、 epoll（LT模式）IO复用，基于 one loop per thread 思想自底向上封装实现 fd event、event loop、 IO 线程、网络地址、TCP acceptor、TCP connection，服务器及客户端等模块。
* 高性能线程池： 基于 lock-free，thread-local queue，任务窃取，线程池自动扩缩容等技术，保持线程池负载均衡，减少竞争及调度开销，提高性能。

### 其他

* 基于 LRU 算法采用池化思想实现 TCP 连接管理，减少资源使用时频繁创建释放产生开销。
* 基于无锁环形队列实现 TCP 读写缓冲区，解决粘包问题。
* 基于红黑树进行定时事件管理，采用 timerfd 和 chrono 标准时间库实现高精度定时器。
* 基于前后端分离思想，采用双缓冲区实现高性能日志组件，使用独立线程实现定时落盘，实现较高性能与可靠性。
* 基于自封装编解码器实现包括自定义 RPC 协议、简化 HTTP 协议在内的可扩展协议解析支持，支持简易 HTTP 服务器及客户端构建。
* 基于红黑树进行服务管理，基于自实现单例分发器实现服务注册、请求解析、任务分发。
* 基于 protobuf 协议实现协议序列化及反序列化支持，兼容标准 protobuf RPC 接口。
* 基于 zoomkeeper 实现分布式 RPC 服务注册中心、统一配置中心，支持服务发现，支持如随机、哈希、加权随机等可配置负载均衡策略。

## 开发环境

### env 1

* Ubuntu 18.04
* xmake v2.7.9+20230515
* gcc version 7.5.0
* vscode + ssh 远程连接开发

### env 2

* Ubuntu 22.04
* xmake v2.8.9+20240321
* gcc version 13.2.0
* vscode + ssh 远程连接开发

### env 3 

* devcloud 云服务器（腾讯实习期间）

## 支撑库

### jsoncpp

采用 json 实现部分配置， 采用 jsoncpp 进行 json 格式解析

https://github.com/open-source-parsers/jsoncpp/tree/update

使用参考：[**[LINK 1]**](https://blog.csdn.net/Worthy_Wang/article/details/107045013) [**[LINK 2]**](https://blog.csdn.net/shuiyixin/article/details/89330529)

### protobuf

使用 protobuf 进行序列化及反序列化解析

https://github.com/protocolbuffers/protobuf

### zoomkeeper

基于 zoomkeeper 的分布式服务注册中心，支持负载均衡

https://github.com/tgockel/zookeeper-cpp


## Others

> 玩具项目，仅用于学习，基本不具备工程级强度
