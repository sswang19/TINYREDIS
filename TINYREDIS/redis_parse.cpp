#include"redis_parse.h"
#include<stdio.h>
#include<string.h>
REDIS_CODE redis_parse::parse_command_line(char* temp, CHECK_STATE& chenkstate)
{
    char* command = strpbrk(temp, " \t");
    // �����������û�пհ��ַ�����"\t"�ַ�����HTTP����������⣨\t�����8���ַ���
    if (!command)
    {
        return BAD_REQUEST;
    }
    *command++ = '\0';
    char* method = temp;
    if (strcasecmp(method, "SET") == 0)
    {
        return SET_REQUEST;
    }
    else if (strcasecmp(method, "GET") == 0)
    {
        return GET_REQUEST;
    }
    else
    {
        return BAD_REQUEST;
    }
}
LINE_STATUS redis_parse::parse_redis_line(char* buffer, int& checked_index, int& read_index)
{
    char temp;
    for (; checked_index < read_index; ++checked_index)
    {
        temp = buffer[checked_index];
        if (temp == '\r')
        {
            //�����һ���ַ���\r��˵�����ܶ���һ��������
            if ((checked_index + 1) == read_index)
                return LINE_DEFECTIVE;
            //�����һ���ַ���\n��˵������һ��������
            else if (buffer[checked_index + 1] == '\n')
            {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        //�����ǰ�ַ�Ϊ\n��Ҳ˵�����ܶ���һ��������
        else if (temp == '\n')
        {

            if ((checked_index > 1) && buffer[checked_index - 1] == '\r')
            {
                buffer[checked_index - 1] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_OK;
        }
    }
    //���������ݶ��������Ҳû������\r�ַ�������LINE_OPEN,��ʾ����Ҫ��ȡ���ݲ��ܽ�һ������
    return LINE_DEFECTIVE;
}
REDIS_CODE redis_parse::parse_obj(char* temp)
{
    //������һ�����У���˵���õ�һ����ȷ����
    if (temp[0] == '\0')
    {
        // return GET_REQUEST;
    }

    //���ƥ���������Դ1 �ɹ�
    if (strncasecmp(temp, "OBJ1:", 5) == 0)
    {

        temp += 5;
        temp += strspn(temp, "\t");
        o.dest = temp;
        return GET_REQUEST;
        /* code */
    }
    else if (strncasecmp(temp, "OBJ2:", 5) == 0)
    {
        temp += 5;
        temp += strspn(temp, "\t");
        o.source = temp;
        return  GET_REQUEST;
    }
    return BAD_REQUEST;
}
REDIS_CODE redis_parse::parse_state_machine(char* buffer, int& checked_index, CHECK_STATE& checkstate, int& read_index, int& start_line)
{
    LINE_STATUS linestatus = LINE_OK;
    REDIS_CODE recode = BAD_REQUEST;
    //��״̬����buffer�ж�ȡ����������
    while ((linestatus = parse_redis_line(buffer, checked_index, read_index)) == LINE_OK)
    {
        // start����line��¼buffer��ʼλ��
        char* temp = buffer + start_line;
        //��¼��һ����ʼλ��
        start_line = checked_index;
        //��¼��״̬����ǰ״̬
        switch (checkstate)
        {
            //��һ״̬������������
        case CHECK_STATE_REQUESTLINE:
        {
            recode = parse_command_line(temp, checkstate);

            if (recode == BAD_REQUEST)
            {
                return BAD_REQUEST;
            }
            else if (recode == SET_REQUEST)
            {
                checkstate = CHECK_STATE_HEAD;
            }
            else if (recode == GET_REQUEST)
            {
                checkstate = CHECK_STATE_HEAD;
            }
            continue;
        }
        case CHECK_STATE_HEAD:
        {

            parse_obj(temp);
            continue;
        }
        default:
        {
            return BAD_REQUEST;
        }
        }
    }
    return recode;
}
inline redis_parse::redis_parse(char* buffer) {

    this->data_read = strlen(buffer);
    this->read_index += data_read;
    this->redis = parse_state_machine(buffer, this->checked_index, this->checkstate, this->read_index, this->start_line);
    if (redis == SET_REQUEST)
    {
        printf("get a right request");
    }
};