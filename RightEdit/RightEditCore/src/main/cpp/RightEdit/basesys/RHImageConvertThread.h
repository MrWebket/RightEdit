//
// Created by Hope liao on 2022/6/21.
//

#ifndef RIGHTEDIT_RHIMAGECONVERTTHREAD_H
#define RIGHTEDIT_RHIMAGECONVERTTHREAD_H

#include <mutex>
#include "../common/RHRefObject.h"
#include <thread>
#include "../common/RHFrameQueue.h"

typedef std::function<void(AVFrame * pFrame)> OnConvertFrameFunc;

class AVFrame;
class RHImageConvertThread: public RHRefObject {
public:
    RHImageConvertThread();
    virtual ~RHImageConvertThread();

public:
    void start();
    void stop();

    void pushFrame(AVFrame * _pInFrame);
    void setRotation(RHRotation _rotation);
    void setCropSize(float width, float height);
    void setConvertFrameCallback(OnConvertFrameFunc func);
    inline uint32_t getSize() {
        return mCacheQueue.size();
    }
private:
    void handleThread();
    void wait();
    void notify();

private:
    RHRotation                      mTargetRoration;
    float                           mCropSize[2];

    OnConvertFrameFunc              mFunc;
    bool                            mAbortRequest;

    std::mutex                      mImageMutex;
    std::condition_variable         mImageCond;

    RHFrameQueue                    mCacheQueue;

    std::thread                   * mThread;
};

#endif //RIGHTEDIT_RHIMAGECONVERTTHREAD_H
