#ifndef __TimeStamp__
#define __TimeStamp__
#include <unistd.h>
#include <iostream>
#include <string>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>

using namespace std ;
static const int MicroSecondsPerSecond =1000*1000;

class TimeStamp
{
public:
    TimeStamp(uint64_t microSeconds)
    :_microSeconds(microSeconds)
    {

    }
    static TimeStamp now(){
        struct timeval tv;
        gettimeofday(&tv, NULL);
        int64_t seconds = tv.tv_sec;
        return TimeStamp(seconds * MicroSecondsPerSecond + tv.tv_usec);
    }
    static TimeStamp nextMicroSeconds(TimeStamp timestamp,uint64_t intervalMicroSeconds){
        return TimeStamp(timestamp.microSeconds() + intervalMicroSeconds);
    }
    static TimeStamp nextSeconds(TimeStamp timestamp,uint64_t intervalSeconds){
        return TimeStamp(timestamp.microSeconds() + intervalSeconds*MicroSecondsPerSecond);
    }
    uint64_t seconds(){
        return _microSeconds/MicroSecondsPerSecond;
    }
    uint64_t microSeconds(){
        return _microSeconds;
    }
//    bool operator < (TimeStamp rhs)
//    {
//      return  this->_microSeconds< rhs.microSeconds();
//    }
//    bool operator > (TimeStamp rhs)
//    {
//      return  this->_microSeconds > rhs.microSeconds();
//    }

//    bool operator==(TimeStamp rhs)
//    {
//      return this->_microSeconds == rhs.microSeconds();
//    }

private:
    uint64_t _microSeconds;//微妙
};
inline bool operator < (TimeStamp lhs,TimeStamp rhs)
{
  return  lhs.microSeconds()< rhs.microSeconds();
}
inline bool operator > (TimeStamp lhs,TimeStamp rhs)
{
  return  lhs.microSeconds() > rhs.microSeconds();
}

inline bool operator ==(TimeStamp lhs,TimeStamp rhs)
{
  return lhs.microSeconds() == rhs.microSeconds();
}
#endif // __TimeStamp__
