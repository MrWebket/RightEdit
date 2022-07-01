//
// Created by Hope liao on 2018/8/1.
//

#ifndef TESTMAGICENGINE_THREADLOCK_H
#define TESTMAGICENGINE_THREADLOCK_H

#include <sys/types.h>

class RHLock{
public:
    void create();

    void destroy();

    void lock();

    void unlock();

    int trylock();

private:
    pthread_mutex_t mutex;
};

#endif //TESTMAGICENGINE_THREADLOCK_H
