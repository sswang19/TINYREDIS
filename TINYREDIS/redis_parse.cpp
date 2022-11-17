#include"redis_parse.h"
#include<stdio.h>
#include<string.h>
REDIS_CODE redis_parse::parse_command_line(char* temp, CHECK_STATE& chenkstate)
{
    char* command = strpbrk(temp, " \t");
    // 如果请求行中没有空白字符或者"\t"字符，则HTTP请求必有问题（\t代表空8个字符）
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
            //如果下一个字符是\r，说明可能读到一个完整行
            if ((checked_index + 1) == read_index)
                return LINE_DEFECTIVE;
            //如果下一个字符是\n。说明读到一个完整行
            else if (buffer[checked_index + 1] == '\n')
            {
                buffer[checked_index++] = '\0';
                buffer[checked_index++] = '\0';
                return LINE_OK;
            }
            return LINE_BAD;
        }
        //如果当前字符为\n。也说明可能读到一个完整行
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
    //若所有内容都分析完毕也没有遇到\r字符，返回LINE_OPEN,表示还需要读取数据才能进一步分析
    return LINE_DEFECTIVE;
}
REDIS_CODE redis_parse::parse_obj(char* temp)
{
    //若遇到一个空行，则说明得到一个正确请求
    if (temp[0] == '\0')
    {
        // return GET_REQUEST;
    }

    //如果匹配操作对象源1 成功
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
    //主状态机从buffer中读取所有完整行
    while ((linestatus = parse_redis_line(buffer, checked_index, read_index)) == LINE_OK)
    {
        // start——line记录buffer起始位置
        char* temp = buffer + start_line;
        //记录下一行起始位置
        start_line = checked_index;
        //记录主状态机当前状态
        switch (checkstate)
        {
            //第一状态，分析请求行
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