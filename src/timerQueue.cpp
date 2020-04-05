#include "mytimer.h"
#include <algorithm>
//retrun fd is timerfd like timeid use to cancel timer

#include "TimerQueue.h"


TimerQueue* TimerQueue::instance =NULL;
TimerQueue::TimerQueue()
{

}

TimerQueue::~TimerQueue()
{

}

TimerQueue *TimerQueue::getInstance()
{
    if(!instance){
        instance = new TimerQueue();
    }
    return instance;
}


void TimerQueue::init()
{
    _timerFd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK);
    if (_timerFd < 0)
    {
        perror("timerfd_create err") ;
    }

}

void TimerQueue::readHandler()
{
    uint64_t howmany;
    ssize_t n = ::read(_timerFd, &howmany, sizeof howmany);
    if (n != sizeof howmany)
    {
      cout<< "TimerQueue::handleRead() reads " << n << endl;
    }
    cout<< "timer run " << n << endl;

    TimeStamp now=TimeStamp::now();

    std::vector<Entry> expired = getExpired(now);

    for (const Entry& it : expired)
    {
      it.second->run();
    }
    reset(expired, now);
    cout<<"_activeTimer size="<<_activeTimer.size()<<endl;
}


vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now)
{
    vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));

    TimerList::iterator end = _activeTimer.lower_bound(sentry);
    TimerList::iterator begin = _activeTimer.begin();

    //copy(_activeTimer.begin(), end, back_inserter(expired));
    int canceledId;
    while(begin!=end)
    {
        canceledId=begin->second->getSequence();
        auto it=find(_cancelingList.begin(),_cancelingList.end(),canceledId);//查看是否失效

        if(it!=_cancelingList.end()){
            delete begin->second;
            _cancelingList.erase(it);
            cout<<"_cancelingList size="<<_cancelingList.size()<<endl;
        }else{
            expired.push_back(*begin);
        }
        begin++;
    }
    _activeTimer.erase(_activeTimer.begin(), end); //删除活跃的定时器
    return expired;
}

void TimerQueue::cancelTimer(int timerid)
{
    _cancelingList.push_back(timerid);
}

bool TimerQueue::insert(Timer *timer)
{
    bool earliestChanged = false;
    TimeStamp when = timer->expiration();

    TimerList::iterator it = _activeTimer.begin();
    if (it==_activeTimer.end() ||when < it->first)
    {
      earliestChanged = true;
    }
    _activeTimer.insert(Entry(when,timer));

    return earliestChanged;
}

int TimerQueue::addTimer(timerCallBack cb, double interval, bool repeat)
{
    Timer* ti = new Timer(move(cb),TimeStamp::now(),interval,repeat);
    if(insert(ti)){
        resetTimerfd(ti->expiration());
    }
}

void TimerQueue::resetTimerfd(TimeStamp expiration)
{
  // wake up loop by timerfd_settime()
  struct itimerspec newValue;
  struct itimerspec oldValue;
  memset(&newValue, 0,sizeof newValue);
  memset(&oldValue, 0,sizeof oldValue);

  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(_timerFd, 0, &newValue, &oldValue);
  if (ret)
  {
    cout << "timerfd_settime()"<<endl;
  }
}

struct timespec TimerQueue::howMuchTimeFromNow(TimeStamp when)
{
  int64_t microseconds = when.microSeconds()- TimeStamp::now().microSeconds();
  if (microseconds < 100)
  {
    microseconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(
      microseconds / MicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
      (microseconds % MicroSecondsPerSecond) * 1000);

  return ts;
}

void TimerQueue::reset(const vector<TimerQueue::Entry> &expired, TimeStamp now)
{
    TimeStamp nextExpire(0);

    for (const Entry& it : expired)
    {
      if (it.second->repeat())
      {
        it.second->reset();
        insert(it.second);
      }
      else
      {
        //过期定时期删�?        cout<<" timerId="<<it.second->getSequence()<<endl;
        delete it.second;
      }
    }

    if (!_activeTimer.empty())
    {
      nextExpire = _activeTimer.begin()->second->expiration();
    }

    if (nextExpire.microSeconds() > 0)
    {
      resetTimerfd(nextExpire);
    }
}


Server::Server()
{
    _epollFd=epoll_create(MAX_EPOLL_COUNT);
    if(_epollFd<0){
        cout<<"epoll_create err!!!!!!!!!!"<<endl;
        exit(0);
    }

}

Server::~Server()
{

}

void Server::changEvent(int fd, int mod, int opt, void *cb)
{
    struct epoll_event ep;
    ep.data.ptr=cb;
    ep.events =mod;
    epoll_ctl(_epollFd,opt,fd,&ep);
}

void Server::addEvent(int fd, int mod)
{
    struct epoll_event ep;
    ep.data.fd=fd;
    ep.events =EPOLLIN|EPOLLET;
    epoll_ctl(_epollFd,EPOLL_CTL_ADD,fd,&ep);
}

void Server::run()
{
    struct epoll_event evens[MAX_EPOLL_COUNT];
    while(1){
        int n=epoll_wait(_epollFd,evens,MAX_EPOLL_COUNT,0);
        if (n > 0 ){
            for(int i=0;i<n;i++)
            {
//                int fd =evens[i].data.fd;
//                uint64_t exp=0; //
//                read(fd,&exp,sizeof(uint64_t));  //be careful in ET mode if no read all data it will not  trigger again
                _readHandler();

            }
            cout<<"havetimer out"<<endl;
        }/*else
           printf("wait timeou \n");*/

    }
}

