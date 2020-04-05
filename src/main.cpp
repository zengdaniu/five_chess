#include "mytimer.h"
#include "timerQueue.h"


int timerId;
class testTimer{
public:
    void hahah(){
        cout<<"timer out hahah"<<endl;
    }
    void heheheh(){
        cout<<"timer out heheheh  "<<testTimer::cunt<<endl;
        testTimer::cunt++;
        if(cunt==10){
            TimerQueue::getInstance()->cancelTimer(timerId);
        }
    }
    static int cunt;

};

int testTimer::cunt = 0;




int main(int argc, char *argv[])
{
    Server server;
    TimerQueue::getInstance()->init();

    server.setHanlder(bind(&TimerQueue::readHandler,TimerQueue::getInstance()));

    server.addEvent(TimerQueue::getInstance()->getTimerFd(),EPOLLIN|EPOLLET);

    testTimer t;

    timerId=TimerQueue::getInstance()->addTimer(std::bind(&testTimer::heheheh, &t),10,true);
    TimerQueue::getInstance()->addTimer(std::bind(&testTimer::hahah, &t),5,false);
    TimerQueue::getInstance()->addTimer(std::bind(&testTimer::hahah, &t),20,false);
    TimerQueue::getInstance()->addTimer(std::bind(&testTimer::hahah, &t),30,false);
    TimerQueue::getInstance()->addTimer(std::bind(&testTimer::hahah, &t),40,false);
    TimerQueue::getInstance()->addTimer(std::bind(&testTimer::hahah, &t),50,false);

    server.run();
    return 0;
}
