#include <iostream>
#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include<deque>
#include <sys/epoll.h>
#include <errno.h>
#include "code_Msg.h"
using namespace std;
class client
{
public:
    int sockfd;
    char *rpc_server_ip;
    unsigned short rpc_server_port;

public:
    // 连接服务器
    bool conn();
    // 初始化列表
    client(char *ip, unsigned short port) : rpc_server_ip(ip), rpc_server_port(port){};
    // 调用
    void loop();
};

bool client::conn()
{
    this->sockfd = socket(AF_INET, SOCK_STREAM, 0); // 创建通信端点：套接字
    if (this->sockfd < 0)
    {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr)); // 初始化服务器地址
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(this->rpc_server_port);
    inet_pton(AF_INET, this->rpc_server_ip, &server_addr.sin_addr);

    int err_log = connect(this->sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)); // 主动连接服务器
}
void client::loop()
{
int num=1;
 while(num++){
        cout<<endl << "6379>";
        string r;
        char *b;
        r="set ";
        
        r.push_back(char(num));
        r.push_back(' ');
        string z="1 ";
        r.append(z);
        codeMsg *p = new codeMsg(r);
        p->code();
        b= p->get_send();
        size_t len = strlen(b);
        send(this->sockfd, b, len, MSG_DONTWAIT);
        char buf[1024];
        size_t a = sizeof(buf);
        read(this->sockfd, buf, a);
        deque<string> re=p->decode_value(buf);
        while(re.empty()!=1)
        {
            cout<<re.front()<<" ";
            re.pop_front();
        }
        cout<<num;
    }
}
int main()
{
    char ip[] = "116.62.56.120";
    unsigned short port = 6379;
    client *c = new client(ip, port);
    c->conn();
    c->loop();
}