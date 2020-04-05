#ifndef LOCK_GUARD_H
#define LOCK_GUARD_H
#include <pthread.h>
class guard
{
public:
    explicit guard(pthread_mutex_t & m):_m(m)
    {
        //printf("construct guard \n");
    }
    void lock(){
        pthread_mutex_lock(&_m);
    }
    ~guard(){
        pthread_mutex_unlock(&_m);
        //printf("destroy \n");
    }

private:
    pthread_mutex_t _m;
};
#endif // LOCK_GUARD_H
