#ifndef REDIS_SERVER_H // 如果没有定义这个宏
#define REDIS_SERVER_H // 定义宏

#include <string>
#include <atomic>

class RedisServer {
public:
    RedisServer(int port);
    void run();
    void shutdown();
private:
    int port;
    int server_socket;
    std::atomic<bool> running;
};
#endif // 结束宏的定义
