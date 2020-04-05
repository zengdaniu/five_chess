#ifndef __My_Timer__
#define __My_Timer__
#include <unistd.h>
#include <iostream>
#include <string>
#include <functional>
#include <stdlib.h>
#include "timeStamp.h"

using namespace std ;
typedef function<void()> timerCallBack;
class Timer
{
public:
    Timer(timerCallBack cb,TimeStamp when,double interval,bool repeat);  //默认是妙为单位
    void run();
    bool repeat() const { return _repeat; }
    TimeStamp newTimeStamp();
    TimeStamp expiration(){
        return _expiration;
    }
    void reset();
    int32_t getSequence(){
        return _sequence;
    }
private:
    timerCallBack _callBack;
    int64_t _interval;
    TimeStamp _expiration;
    bool _repeat;
    int32_t _sequence;

    static uint32_t sequence;

};

#endif // __My_Timer__
