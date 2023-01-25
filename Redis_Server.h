
// 这个对象的功能
/*
    1.接收客户端字符串返回信息
    //如何封装事件循环 //忽略开启绑定监听，
    //分布式事务 和 数据库事务  是割裂的，同样使用IO多路复用，两个线程负责不同业务
    //分布式事务采用RPC框架?,本地计算环回从机，，
*/

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <sys/stat.h>
#include <string.h>
#include <string>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/uio.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/epoll.h>
using namespace std;
class Redis_Server
{
public:
    void close_conn();      // 关闭连接
    void main_event_loop(); // redis服务主体事件循环
    void cluster_service(); // redis分布式服务事件循环,暂时不开通日志传递服务，只有写同步，心跳测试，选主服务
    Redis_Server(string ip, unsigned port, string master_ip, unsigned master_port);
    ~Redis_Server();
    void lisport();//
private:
    
    int lfd;
    int epfd;
    struct epoll_event epev;
    struct epoll_event epevs[1024];
    int server_id;
    string server_ip;
    unsigned server_port;
    string server_master_ip;
    int server_master_id;
    unsigned server_master_port;
    
};