#ifndef SKIPLIST_H
#define SKIPLIST_H
#include<map>
#include<string>
#include<deque>
#include<set>
//完成存储命令，读取命令，一个哈希表，
using namespace std;

class skiplist
{
private:
    /* data */
    map <string,deque<string> >sklist;
    
public:
    skiplist();
    ~skiplist();
    bool setkv(string key,deque<string> value);
    bool isexist(string key);
    deque<string> getvalue(string key);
};
skiplist::skiplist()
{

}
skiplist::~skiplist()
{

}
bool skiplist::setkv(string key,deque<string> value)
{
    deque<string> s;
    while(value.empty()!=1)
    {
        s.push_back(value.front());
        value.pop_front();
    }
    this->sklist.insert(pair<string,deque<string> >(key,s));
    return true;
}

bool skiplist::isexist(string key)
{
     //键值存在
 if(this->sklist.find(key)==this->sklist.end())
    return false;
 return true;
}

deque<string> skiplist::getvalue(string key)
{
    deque<string> d = this->sklist[key];
    return d;  
}

#endif