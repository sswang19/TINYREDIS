
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
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <stdarg.h>
#include <iostream>
#include <errno.h>
#include <sys/uio.h>
#include "setting.h"
#include "Redis_Server.h"
#include "code_Msg.h"
#include "skiplist.h"
using namespace std;
Redis_Server::Redis_Server(string ip, unsigned port, string master_ip, unsigned master_port)
{
    this->server_ip = ip;
    this->server_port = port;
    this->server_master_ip = master_ip;
    this->server_master_port = master_port;
}
void Redis_Server::lisport()
{
    // 开始监听端口。reactor模式注册epoll监听事件
    this->lfd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in saddr;
    saddr.sin_port = htons(6379);

    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = INADDR_ANY;
    // 将监听描述符设置为非阻塞
    set_non_blocking(lfd);
    int opt = 1;
    // sockfd为需要端口复用的套接字
    setsockopt(this->lfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&opt, sizeof(opt));
    bind(this->lfd, (struct sockaddr *)&saddr, sizeof(saddr));

    listen(this->lfd, 8);
    this->epfd = epoll_create(100);

    this->epev.events = EPOLLIN;

    this->epev.data.fd = lfd;

    epoll_ctl(this->epfd, EPOLL_CTL_ADD, this->lfd, &this->epev);
}
void Redis_Server::cluster_service()
{
}

void Redis_Server::main_event_loop()
{
    int ret, flag = 0;
    skiplist sklist;
    struct sockaddr_in cliaddr;
    while (true)
    {
        ret = epoll_wait(this->epfd, this->epevs, 1024, -1);
        if (ret == -1)
        {
            perror("epoll_wait");
            exit(-1);
        }
        for (int i = 0; i < ret; ++i)
        {
            int curfd = epevs[i].data.fd;
            char clientip[16];
            if (curfd == this->lfd)
            {
                socklen_t len = sizeof(cliaddr);
                int cfd = accept(this->lfd, (struct sockaddr *)&cliaddr, &len);

                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clientip, sizeof(clientip));
                unsigned short clientPort = ntohs(cliaddr.sin_port);
                this->epev.events = EPOLLIN;
                this->epev.data.fd = cfd;
                set_non_blocking(curfd);
                epoll_ctl(this->epfd, EPOLL_CTL_ADD, cfd, &this->epev);
            }
            else
            {
                if (this->epevs[i].events & EPOLLOUT)
                {
                    continue;
                }
                inet_ntop(AF_INET, &cliaddr.sin_addr.s_addr, clientip, sizeof(clientip));
                unsigned short clientport = ntohs(cliaddr.sin_port);

                // 数据到达
                char buf[1024] = {0};
                int len = read(curfd, buf, sizeof(buf));

                if (len == -1)
                {
                    perror("recv");
                    exit(-1);
                }
                else if (len == 0)
                {
                    // 客户端断开连接；删除监听的描述符
                    cout << "断开连接";
                    epoll_ctl(this->epfd, EPOLL_CTL_DEL, curfd, NULL);
                    close(curfd);
                }
                else if (len > 0)
                {
                    // 生产者添加任务

                    // 解析buf,返回命令信息, 得到

                    codeMsg *p = new codeMsg(buf, 1);
                    p->decode();
                    // p->printkv();
                    string command, key, va;
                    deque<string> value;
                    command = p->getcommand();
                    key = p->getkey();
                    value = p->getvalue();
                 //   cout << "recv command" << command << "key" << key;
                    const char *resp = new char[1024];
                    // 写事件。自己写，分发从机
                    if (command == "set")
                    {
                        if (sklist.setkv(key, value))
                        {
                            string ok = "OK\r\n";
                            resp = ok.c_str();
                        }
                        else
                        {
                            string fa = "FALSE\r\n";
                            resp = fa.c_str();
                        }
                        write(curfd, resp, strlen(resp) + 1);
                    }
                    else // 读get
                    {
                        if (sklist.isexist(key))
                        {
                            deque<string> d = sklist.getvalue(key);
                            d.pop_front();
                            resp = p->parsevalue(d);
                            
                        }
                        else
                        {
                            string er = "key doesn't exist\r\n";
                            resp = er.c_str();
                        }
                        write(curfd, resp, strlen(resp) + 1);
                    }
                }
            }
        }
    }
}

Redis_Server::~Redis_Server()
{
    close(this->lfd);
    close(this->epfd);
}

int main()
{
    Redis_Server *r = new Redis_Server("fda", 22, "dsa", 21);
    r->lisport();
    r->main_event_loop();
}