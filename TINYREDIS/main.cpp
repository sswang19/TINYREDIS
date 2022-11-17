#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<string>
#include <sys/epoll.h>
#include"redis_parse.h"
using namespace std;
struct client_info {
    char client_ip[16];
    char client_port[6];
    string con_time;
};
class time_get {
public:
    int year, month, minutes, second, millisecond;

};
int main()
{

    // 创建socket
    int cli_num = 0;
    int lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(9999);
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;

    // 绑定
    bind(lfd, (struct sockaddr*)&saddr, sizeof(saddr));

    // 监听
    listen(lfd, 8);

    // 调用epoll_create()创建一个epoll实例
    int epfd = epoll_create(100);

    // 将监听的文件描述符相关的检测信息添加到epoll实例中
    struct epoll_event epev;
    epev.events = EPOLLIN;
    epev.data.fd = lfd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, lfd, &epev);
    struct sockaddr_in cliaddr;
    struct epoll_event epevs[1024];
    char clientIP[16];
    while (1)
    {

        int ret = epoll_wait(epfd, epevs, 1024, -1);
        if (ret == -1)
        {
            perror("epoll_wait");
            exit(-1);
        }

        printf("ret = %d\n", ret);

        for (int i = 0; i < ret; i++)
        {

            int curfd = epevs[i].data.fd;

            if (curfd == lfd)
            {
                // 监听的文件描述符有数据达到，有客户端连接

                socklen_t len = sizeof(cliaddr);
                int cfd = accept(lfd, (struct sockaddr*)&cliaddr, &len);
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(cliaddr.sin_port);
                printf("%s %d客户端连接成功\n", clientIP, clientPort);
                cli_num++;
                epev.events = EPOLLIN;
                epev.data.fd = cfd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, cfd, &epev);
            }
            else
            {
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clientIP, sizeof(clientIP));
                unsigned short clientPort = ntohs(cliaddr.sin_port);
                if (epevs[i].events & EPOLLOUT)
                {
                    continue;
                }
                // 有数据到达，需要通信
                char *buf= { 0 };
                int len = read(curfd, buf, sizeof(buf));

                if (len == 0)
                {
                    printf("%s %d客户端断开连接\n", clientIP, clientPort);
                    printf("client_num:%d\n", cli_num);
                    epoll_ctl(epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else
                {
                    redis_parse  *p = new redis_parse(buf);
                    write(curfd, buf, strlen(buf) + 1);
                }
            }
        }
    }

    close(lfd);
    close(epfd);
    return 0;
}