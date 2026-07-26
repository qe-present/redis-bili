#include "../include/RedisServer.h"
#include <iostream>

#include <string>
int main(int argc, char* argv[]) {
    // argv 命令行参数的字符串书籍，argv[0] 表示程序名。argv[1] 表示第一个参数，argv[2] 表示第二个参数。
    int port=7379; //default
    if(argc>=2)
        // atoi 把string变成数字
        port = std::stoi(argv[2]);
    RedisServer server(port);
    return 0;
}
