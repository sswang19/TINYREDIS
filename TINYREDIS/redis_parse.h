#pragma once
#define buffer_size 4096;

//��״̬�����ֿ���״̬����ʾ��ǰ���ڷ��������У���ǰ���ڷ���ͷ���ֶ�
enum CHECK_STATE
{
    CHECK_STATE_REQUESTLINE = 0,
    CHECK_STATE_HEAD
};
//��״̬������״̬ �ж�ȡ ����״̬����ȡ�����У��г���������ݲ�����
enum LINE_STATUS
{
    LINE_OK,
    LINE_BAD,
    LINE_DEFECTIVE
};
//����������REDIS����Ľ���������������set������get
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
    // ��ǰ�Ѿ���ȡ�˶����ֽڵĿͻ�����
    int read_index = 0;
    // ��ǰ�Ѿ��������˶����ֽڵĿͻ�����
    int checked_index = 0;
    // ����buffer�е���ʼλ��
    int start_line = 0;
    // ������״̬���ĳ�ʼ״̬
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