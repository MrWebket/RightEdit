//
// Created by macbookpro on 8/20/21.
//

#ifndef V6PLAYER_SRPACKETQUEUE_H
#define V6PLAYER_SRPACKETQUEUE_H


#include "../common/RHRefObject.h"
#include <deque>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
}
class RHPacketQueue : public RHRefObject {
public:
    RHPacketQueue();
    ~RHPacketQueue();
public:
    void setFlushPacket(AVPacket * pkt);
    void push_front(AVPacket * pkt);
    void push_back(AVPacket * pkt);
    AVPacket * front();
    AVPacket * back();
    void pop_front();
    void pop_back();
    void readAndPop(AVPacket ** pkt);
    void read(AVPacket ** pkt);
    void flush();
    void pushFlushPacket();
    bool isFlushPacket(AVPacket * packet);

    int size();
    bool empty();
private:
    std::deque<AVPacket *>  mQueue;
    std::mutex              mMutex;
    AVPacket              * mFlushPkt;
};
#endif //V6PLAYER_SRPACKETQUEUE_H