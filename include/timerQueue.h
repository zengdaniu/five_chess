#ifndef __Time_Queue__
#define __Time_Queue__
#include <functional>
#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <string.h>
#include <set>
#include <memory>
#include <list>
#include <vector>
#include "mytimer.h"
using namespace std ;
static const int MAX_EPOLL_COUNT =20;

typedef function<void()> ReadHandler;

class Server{
public:
    Server();
    ~Server();
    void changEvent(int fd ,int mod ,int opt,void * cb);
    void addEvent(int fd ,int mod);
    void run();
    void setHanlder(ReadHandler hanlder){
        _readHandler=hanlder;
    }
private:
    int32_t _epollFd;
    ReadHandler _readHandler;
};



class TimerQueue
{
public:
    ~TimerQueue();
    TimerQueue();
    static TimerQueue* getInstance();
    void init();
    void readHandler();
    int addTimer(timerCallBack cb,double interval,bool repeat);
    void cancelTimer(int timerid);
    int32_t getTimerFd(){
        return _timerFd;
    }
private:
    bool insert(Timer* timer);
    void resetTimerfd(TimeStamp expiration);
    struct timespec howMuchTimeFromNow(TimeStamp when);

    typedef std::pair<TimeStamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;
    void reset(const vector<Entry>& expired, TimeStamp now);
    vector<TimerQueue::Entry> getExpired(TimeStamp now);


    int32_t _timerFd;
    TimerList _activeTimer;
    list<int> _cancelingList;

    static TimerQueue* instance;
};

#endif // __Time_Queue__
