//
// Created by Hope liao on 2022/6/21.
//

#include <common/RHPNGEncoder.h>
#include "RHImageEncoderThread.h"
#include "../common/RHAppDef.h"
#include "../common/RHCommonDefine.h"

RHImageEncoderThread::RHImageEncoderThread() {
    mCacheQueue.clear();
    mAbortRequest = false;
    mThread = nullptr;
    mEncoderPath = "";
    mIndex = 0;
}

RHImageEncoderThread::~RHImageEncoderThread() noexcept {
    stop();
    mCacheQueue.clear();
    mAbortRequest = false;
    mThread = nullptr;
    mEncoderPath = "";
    mIndex = 0;
}

void RHImageEncoderThread::start() {
    mAbortRequest = false;
    mThread = new std::thread(&RHImageEncoderThread::handleThread, this);
}

void RHImageEncoderThread::stop() {
    mAbortRequest = true;
    notify();
    SAFE_DELETE_THREAD(mThread)
}

void RHImageEncoderThread::pushFrame(AVFrame *_pInFrame) {
    AVFrame * dstFrame = av_frame_clone(_pInFrame);
    mCacheQueue.push_back(dstFrame);

    notify();
}

bool RHImageEncoderThread::isEncodeComplete() {
    if(mCacheQueue.empty()) {
        return true;
    }
    return false;
}

void RHImageEncoderThread::setEncoderPath(const char *_path) {
    mEncoderPath = _path;
}

void RHImageEncoderThread::handleThread() {
    ALOGI("image encode thread start.");
    int index = 0;
    while (true) {
        AGAIN:
        if(mAbortRequest) {
            goto END;
        }

        if(mCacheQueue.empty()) {
            wait();
            goto AGAIN;
        }
        AVFrame * pInFrame = mCacheQueue.read_front_pop();
        if(pInFrame) {
            std::string outputPath = mEncoderPath;
            outputPath.append("/");
            outputPath.append("thumbnail_");
            outputPath.append(std::to_string(index));
            outputPath.append(".png");

            RHPNGEncoder encoder = {};
            int ret = encoder.encodeImage(outputPath.c_str(), pInFrame);
            if(ret == RH_SUCCEED) {
                index++;
                SAFE_FREE_AVFRAME(pInFrame)
            }
        }
    }
    END:
    ALOGI("image encode thread done.");
    mCacheQueue.clear();
}

void RHImageEncoderThread::wait() {
    SAFE_WAIT_THREAD(mImageMutex, mImageCond)
}

void RHImageEncoderThread::notify() {
    SAFE_NOTIFY_THREAD(mImageMutex, mImageCond)
}