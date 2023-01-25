
#include<string>
#include<deque>
using namespace std;
class codeMsg{
    public:
        codeMsg(string msg);
        codeMsg(char recv[],int n);
        string getcommand();
        string getkey();
        deque<string> getvalue();
        char* get_send();
        bool code();
        bool decode();
        void print();
        char *parsevalue(deque<string> d);
        void printkv();
        deque<string> decode_value(char buf[]);
    private:
        string command;
        string Msg;
        int len;
        char send[1000];
        string key;
        deque<string> value;
};
