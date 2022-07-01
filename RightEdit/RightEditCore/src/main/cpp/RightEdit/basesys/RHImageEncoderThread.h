//
// Created by Hope liao on 2022/6/21.
//

#ifndef RIGHTEDIT_RHIMAGEENCODERTHREAD_H
#define RIGHTEDIT_RHIMAGEENCODERTHREAD_H

#include "../common/RHRefObject.h"
#include <mutex>
#include "../common/RHFrameQueue.h"
#include <thread>
#include <vector>

class AVFrame;
class RHImageEncoderThread: public RHRefObject {
public:
    RHImageEncoderThread();
    virtual ~RHImageEncoderThread();
public:
    void start();
    void stop();

    void pushFrame(AVFrame * _pInFrame);
    bool isEncodeComplete();
    void setEncoderPath(const char * _path);
private:
    void handleThread();
    void wait();
    void notify();

private:
    std::string                     mEncoderPath;

    bool                            mAbortRequest;

    std::mutex                      mImageMutex;
    std::condition_variable         mImageCond;

    RHFrameQueue                    mCacheQueue;

    std::thread                   * mThread;

    int                             mIndex;
};
#endif //RIGHTEDIT_RHIMAGEENCODERTHREAD_H
