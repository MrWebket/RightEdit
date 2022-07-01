//
// Created by macbookpro on 8/20/21.
//

#ifndef V6PLAYER_SRFRAMEQUEUE_H
#define V6PLAYER_SRFRAMEQUEUE_H

#include "../common/RHRefObject.h"
#include <deque>
#include <mutex>

extern "C" {
#include <libavutil/frame.h>
}
class RHFrameQueue : public RHRefObject {
public:
    RHFrameQueue();
    ~RHFrameQueue();

public:
    void push_front(AVFrame * _pInFrame);
    void push_back(AVFrame * _pInFrame);
    bool push_back(int serial, AVFrame * _pInFrame);
    AVFrame * front();
    AVFrame * back();

    void pop_front();
    void pop_back();
    void read_front_pop(AVFrame ** _pInFrame);
    AVFrame  * read_front_pop();
    void read_front(AVFrame ** _pInFrame);
    AVFrame * clone_front();
    int64_t   getNextPts();
    int64_t  getFrontPts();
    int flush();
    void clear();

    bool isFlushFrame(AVFrame * frame);
    bool isFrontFlush();
    int size();
    bool empty();
    int getSerial();
private:
    std::deque<AVFrame *>   mQueue;
    std::mutex              mMutex;

    AVFrame               * mFlushFrame;
    int                     mSerial;
};

#endif //V6PLAYER_SRFRAMEQUEUE_H
