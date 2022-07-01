
#ifndef LIBV6LIVE_UTILS_H
#define LIBV6LIVE_UTILS_H

#include <memory.h>
#include <stdlib.h>
#include <unistd.h>

#include <inttypes.h>
#include <sys/socket.h>
#include <endian.h>
#include <sys/time.h>
#include "../common/RHCommonDefine.h"

uint32_t MixINT16(uint8_t *pDst, uint8_t *pSrc1, uint8_t *pSrc2, uint32_t dwLength);

uint32_t P2U32(unsigned char **pSrc);
uint32_t P2U32(unsigned char *pSrc);

inline uint32_t GetNowMS()
{
    timeval time;
    gettimeofday(&time, NULL);
    uint32_t ms = static_cast<uint32_t>(time.tv_sec * 1000 + time.tv_usec / 1000);
    return ms;
}

inline long GetNowNS()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_usec;
}

inline uint32_t GetNowSec()
{
    timeval time;
    gettimeofday(&time, NULL);
    return static_cast<uint32_t>(time.tv_sec);
}

inline RHRotation GetRotation(int angle) {
    switch (angle) {
        case 90:
            return ROTATE_90;
        case 180:
            return ROTATE_180;
        case 270:
            return ROTATE_270;
        default:
            return ROTATE_0;
    }
}

inline uint64_t GetThreadId() {
    uint64_t id = -1;
#ifdef V6_IOS
    pthread_threadid_np(0, &id);
#else
    id = gettid();
#endif
    return id;
}


bool PortAvailable(uint16_t port);

void SleepMS(int ms);

#endif //LIBV6LIVE_UTILS_H
