#include "RedisServer.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
static RedisServer* globalServer;
//这是 C++ 的成员初始化列表（Member Initializer List），
// 在 {} 函数体执行之前完成成员变量的初始化。
RedisServer::RedisServer(int port): port(port),server_socket(-1),running(true){
    globalServer=this;
}
// :: 是 C++ 的作用域解析运算符（Scope Resolution Operator）
// 这是 RedisServer 类的 shutdown 成员函数
void RedisServer::shutdown() {
    running=false;
    if(server_socket!=-1){
        close(server_socket);
    }
    std::cout<<"Server shutdown Complete\n";
}
void RedisServer::run() {
    // AF_INET 表示使用 IPv4 协议
    // SOCK_STREAM 表示使用 TCP 协议
    // 0 表示使用默认的协议
    // 向操作系统申请创建一个 基于 IPv4 的 TCP 套接字，用于网络通信。
    // 成功：返回一个非负整数（文件描述符），存入 server_socket
    // 失败：返回 -1
    server_socket=socket(AF_INET,SOCK_STREAM,0);

    if(server_socket<0){
        std::cerr<<"Failed to create socket\n";
        return;
    }

}
