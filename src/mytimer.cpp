#include "mytimer.h"
//retrun fd is Timerfd like timeid use to cancel Timer


uint32_t Timer::sequence = 0;
Timer::Timer(timerCallBack cb, TimeStamp when,double interval, bool repeat)
:_callBack(move(cb)),
 _interval(interval*MicroSecondsPerSecond),
 _repeat(repeat),
 _expiration(TimeStamp::nextMicroSeconds(when,_interval))
{
    _sequence =Timer::sequence;
    ++ Timer::sequence;
}

void Timer::run()
{
    _callBack();
}

TimeStamp Timer::newTimeStamp()
{
    return TimeStamp::nextMicroSeconds(_expiration,_interval);
}

void Timer::reset()
{
    _expiration=newTimeStamp();
}
