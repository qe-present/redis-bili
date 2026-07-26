#include "RedisServer.h"
#include <iostream>
#include <sys/socket.h>  // socket(), bind(), listen(), accept() 等网络 API
#include <unistd.h>      // close() 系统调用
#include <netinet/in.h>   // sockaddr_in, INADDR_ANY
#include <arpa/inet.h>    // htons, htonl, inet_addr 等
// 全局指针，用于信号处理等场景获取服务器实例
static RedisServer* globalServer;

/**
 * 构造函数：使用成员初始化列表在函数体执行前完成成员变量初始化
 * @param port 服务器监听端口
 */
RedisServer::RedisServer(int port):
    port(port),           // 初始化端口
    server_socket(-1),    // 初始化为 -1，表示尚未创建套接字
    running(true)         // 服务器初始状态为运行中
{
    // 将当前实例地址赋给全局指针，便于外部（如信号处理器）访问
    globalServer = this;
}

/**
 * 关闭服务器
 * :: 是 C++ 作用域解析运算符，表明 shutdown 是 RedisServer 类的成员函数
 */
void RedisServer::shutdown() {
    running = false;
    // 如果套接字已创建，则关闭它释放资源
    if (server_socket != -1) {
        close(server_socket);
    }
    std::cout << "Server shutdown Complete\n";
}

/**
 * 启动服务器主循环
 */
void RedisServer::run() {
    // socket()：向操作系统申请创建套接字
    // AF_INET    → 使用 IPv4 地址族（Address Family Internet）
    // SOCK_STREAM → 使用面向连接的流式套接字（即 TCP 协议）
    // 0          → 由系统自动选择默认协议（此处为 TCP）
    // 返回值：成功返回非负的文件描述符（fd），失败返回 -1
    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // 检查套接字是否创建成功
    if (server_socket < 0) {
        std::cerr << "Failed to create socket\n";
        return;
    }
    int opt=1;
    // server_socket 套接字文件描述符
    // SOL_SOCKET 套接字选项级别
    // SO_REUSEADDR 允许地址重用选项
    // &opt 指向选项值的指针
    // sizeof(opt) 选项值的大小
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // ============================================================
    // 2. 绑定地址：将套接字与指定的 IP 和端口关联
    // ============================================================
    // bind() 把 server_socket "钉" 到 serverAddr 描述的地址上，
    // 之后操作系统收到的目标为该端口的数据包才会交给这个套接字。
    //
    // 参数说明：
    //   server_socket                → 要绑定的套接字文件描述符
    //   (struct sockaddr*)&serverAddr → 强制转换为通用地址结构体指针
    //                                   （Berkeley Socket API 的历史遗留设计）
    //   sizeof(serverAddr)           → 地址结构体的字节长度
    //
    // 返回值：成功返回 0，失败返回 -1 并设置 errno
    // ============================================================
    if (bind(server_socket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind socket\n";   // 常见原因：端口被占用或权限不足(<1024)
        return;                                    // 绑定失败，无法继续，直接退出
    }

    // ============================================================
    // 3. 进入监听状态：将套接字设为被动模式，准备接收连接请求
    // ============================================================
    // listen() 告诉内核：这个套接字是用来"接听"外来连接的。
    // 内核会为此套接字维护两个队列：
    //   · 半连接队列（SYN队列）：收到 SYN，尚未完成三次握手
    //   · 全连接队列（accept队列）：已完成三次握手，等待 accept() 取走
    //
    // 参数说明：
    //   server_socket → 已绑定地址的套接字
    //   10            → backlog，全连接队列的最大长度。
    //                   如果队列满，新的连接会被内核拒绝（客户端收到 ECONNREFUSED）
    //                   实际上限还受系统参数 net.core.somaxconn 影响
    //
    // 返回值：成功返回 0，失败返回 -1
    // ============================================================
    if (listen(server_socket, 10) < 0) {
        std::cerr << "Failed to listen on socket\n";
        return;                                    // 监听失败，服务器无法对外服务
    }
    std::cout << "Server listening on port " << port << "\n";

}
