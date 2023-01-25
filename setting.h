#ifndef SETTING_H
#define SETTING_H
#include<iostream>
#include<string>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include<fcntl.h>
#include <sys/epoll.h>
void set_non_blocking(int &fd)
{
    int flag=fcntl(fd,F_GETFL,0);
    fcntl(fd,F_SETFL,flag|O_NONBLOCK);
}
#endif