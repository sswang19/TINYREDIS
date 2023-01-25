
#include <string>
#include <iostream>
#include <stdio.h>
#include <deque>
#include <string.h>
#include "code_Msg.h"
using namespace std;
string codeMsg::getcommand(){
    return this->command;
}
string codeMsg::getkey()
{
    return this->key;
}
 deque<string> codeMsg::getvalue()
 {
    return this->value;
 }
codeMsg::codeMsg(char recv[],int n)
{
    //
    int size=strlen(recv);
    for(int i=0;i<size;++i)
    {
        send[i]=recv[i];
    }
}
codeMsg::codeMsg(string msg)
{
    this->Msg = msg;
    this->len = msg.length();
}

bool codeMsg::code()
{
    string send;
    int flag = 0;
    std::deque<char> buf;
    std::deque<std::string> msg_deque;
    for (int i = 0; i <= this->len; ++i)
    {
        // 这里要对msg进行解析，遇到空格就弹出空格之前的字符串
        if (this->Msg[i] == ' ' || i == this->len)
        {
            // buf中全部弹出,整合为一个字符串类型deque，
            std::string temp;
            while (buf.empty() != 1)
            {
                temp.push_back(buf.front());
                buf.pop_front();
            }
            msg_deque.push_back(temp);
        }

        // 压栈
        else
            buf.push_back(this->Msg[i]);
    }

    while (msg_deque.empty() != 1)
    {
        if (flag == 0)
        {
            send.push_back('O');
            send.push_back(msg_deque.front().length() + '0');
            send.append(msg_deque.front() + "\r\n");
            msg_deque.pop_front();
            flag++;
        }
        else if (flag == 1)
        {
            send.push_back('K');
            send.push_back(msg_deque.front().length() + '0');
            send.append(msg_deque.front() + "\r\n");
            msg_deque.pop_front();
            flag++;
        }
        else if (flag == 2)
        {
            send.push_back('*');
            send.push_back(msg_deque.size() + '0');
            send.append("\r\n");
            flag++;
        }
        else
        {
            send.push_back('$');
            send.push_back(msg_deque.front().length() + '0');
            send.append(msg_deque.front() + "\r\n");
            msg_deque.pop_front();
            flag++;
        }
    }
    char charbuf[100];
    for (int i = 0; i <= send.length(); ++i)
    {
        charbuf[i] = send[i];
    }
    char sent[send.length()];
    strcpy(sent, charbuf);
    strcpy(this->send, sent);
}
void codeMsg::print()
{
    int i = 0;
    while (this->send[i] != '\0')
    {
        printf("%c", this->send[i++]);
    }
}
bool codeMsg::decode()
{
    int i = 0;
    deque<string> r, write;
    deque<char> recv;
    while (this->send[i] != '\0')
    {
        if (this->send[i] == '\r')
        {
            string a;
            if (recv.front() != '*')
            {
                recv.pop_front();
                recv.pop_front();
                while (recv.empty() != 1)
                {
                    a.push_back(recv.front());
                    recv.pop_front();
                }
                r.push_back(a);
            }
            else
            {
                recv.pop_front();
                while (recv.empty() != 1)
                {
                    a.push_back(recv.front());
                    recv.pop_front();
                }
                r.push_back(a);
            }
            i += 2;
        }
        recv.push_back(this->send[i++]);
    }
    string command = r.front();
    if (command == "set")
    {
        this->command = r.front();
        r.pop_front();
        this->key = r.front();
        r.pop_front();
        while (r.empty() != 1)
        {
            this->value.push_back(r.front());
            r.pop_front();
        }
    }
    else if (command == "get")
    {
        this->command = r.front();
        r.pop_front();
        this->key = r.front();
        r.pop_front();
    }
}
void codeMsg::printkv()
{
    cout << "command is " << this->command << endl;
    cout << "this is key  " << this->key << endl;

    if (this->command == "set")
    {

        cout << "this is value ";
        this->value.pop_front();
        while (this->value.empty() != 1)
        {
            cout << this->value.front() << "\t";
            this->value.pop_front();
        }
    }
}
deque<string> codeMsg::decode_value(char buf[])
{
    
    int i = 0;
    deque<string> r, write;
    deque<char> recv;

    while (buf[i] != '\0')
    {
        if (buf[i] == '\r')
        {
            string a;
            if (recv.front() != '*')
            {
                recv.pop_front();
                recv.pop_front();
                while (recv.empty() != 1)
                {
                    a.push_back(recv.front());
                    recv.pop_front();
                }
                r.push_back(a);
            }
            else
            {
                recv.pop_front();
                while (recv.empty() != 1)
                {
                    a.push_back(recv.front());
                    recv.pop_front();
                }
                r.push_back(a);
            }
            i += 2;
        }
        recv.push_back(buf[i++]);
    }
    
    if(buf[0]=='*')
    {
        r.pop_front();
    }
    return r;
}
char *codeMsg::parsevalue(deque<string> d)
{
    char *buf=new char[1024];
    int f=4;
    string b;
    b.push_back('*');
    b.push_back(char(d.size())+'0');
    b.append("\r\n");
   while(d.empty()!=1)
    {
        b.append(d.front()+"\r\n");
        d.pop_front();
    }
    strcpy(buf,b.c_str());
    return buf;
}
char * codeMsg::get_send()
{
    return this->send;
}