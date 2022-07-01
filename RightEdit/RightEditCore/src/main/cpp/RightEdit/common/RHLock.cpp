//
// Created by Hope liao on 2018/8/1.
//

#include <pthread.h>
#include "RHLock.h"

void RHLock::create() {
    pthread_mutex_init(&mutex, nullptr);
}

void RHLock::destroy() {
    pthread_mutex_destroy(&mutex);
}

void RHLock::lock() {
    pthread_mutex_lock(&mutex);
}

void RHLock::unlock() {
    pthread_mutex_unlock(&mutex);
}

int RHLock::trylock() {
    return pthread_mutex_trylock(&mutex);
}