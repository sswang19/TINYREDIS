#pragma once
#define buffer_size 4096;

//主状态机两种可能状态，表示当前正在分析请求行，当前正在分析头部字段
enum CHECK_STATE
{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEAD
};
//从状态机三种状态 行读取 三个状态，读取完整行，行出错和行数据不完整
enum LINE_STATUS
{
    LINE_OK,
    LINE_BAD,
    LINE_DEFECTIVE
};
//服务器处理REDIS请求的结果，请求出错，请求set，请求get
enum REDIS_CODE
{
    SET_REQUEST,
    GET_REQUEST,
    BAD_REQUEST
};
class redis_parse
{
    REDIS_CODE redis;
    int data_read = 0;
    // 当前已经读取了多少字节的客户数据
    int read_index = 0;
    // 当前已经分析完了多少字节的客户数据
    int checked_index = 0;
    // 行在buffer中的起始位置
    int start_line = 0;
    // 设置主状态机的初始状态
    char *buffer;
    inline redis_parse(char* buffer);
    CHECK_STATE checkstate = CHECK_STATE_REQUESTLINE;
    virtual REDIS_CODE parse_state_machine(char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line) {};
    virtual REDIS_CODE parse_obj(char* temp) {};
    virtual REDIS_CODE parse_command_line(char* temp, CHECK_STATE& chenkstate) {};
    virtual LINE_STATUS parse_redis_line(char* buffer, int& checked_index, int& read_index) {};

};

class obj
{
public:
    char* dest;
    char* source;
};
obj o;