//
// Created by Hope liao on 2022/6/21.
//

#include <cmath>
#include "../common/RHYUVConvert.h"
#include "RHImageConvertThread.h"
#include "../common/RHAppDef.h"
#include "../data/RHFrameData.h"

extern "C" {
#include <libavutil/imgutils.h>
}

RHImageConvertThread::RHImageConvertThread() {
    mAbortRequest = false;
    mFunc = nullptr;
    mThread = NULL;
    mTargetRoration = ROTATE_0;
    mCacheQueue.clear();

    mCropSize[0] = 1.0F;
    mCropSize[1] = 1.0F;
}

RHImageConvertThread::~RHImageConvertThread() noexcept {
    stop();
    mFunc = nullptr;
    mTargetRoration = ROTATE_0;
    mCacheQueue.clear();

    mCropSize[0] = 1.0F;
    mCropSize[1] = 1.0F;
}

void RHImageConvertThread::start() {
    mAbortRequest = false;
    mThread = new std::thread(&RHImageConvertThread::handleThread, this);
}

void RHImageConvertThread::pushFrame(AVFrame *_pInFrame) {
    AVFrame * dstFrame = av_frame_clone(_pInFrame);
    mCacheQueue.push_back(dstFrame);
    notify();
}

void RHImageConvertThread::setRotation(RHRotation _rotation) {
    mTargetRoration = _rotation;
}

void RHImageConvertThread::setCropSize(float width, float height) {
    mCropSize[0] = width;
    mCropSize[1] = height;
}

void RHImageConvertThread::setConvertFrameCallback(OnConvertFrameFunc func) {
    mFunc = std::move(func);
}

void RHImageConvertThread::stop() {
    mAbortRequest = true;
    notify();
    SAFE_DELETE_THREAD(mThread)
}

void RHImageConvertThread::handleThread() {
    ALOGI("frame handle thread start.");
    RHYUVConvert yuvConvert = {};
    yuvConvert.setScale(std::fmax(mCropSize[0], mCropSize[1]));
    yuvConvert.setTargetFormat(RH_PIX_FORMAT_RGBA);
    yuvConvert.setRotation(mTargetRoration);

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
            RHFrameData * pOutDataBuffer = NULL;
            bool bConvertState = yuvConvert.convertFrame(pInFrame, &pOutDataBuffer);
            if(bConvertState && pOutDataBuffer) {
                AVPixelFormat format = pOutDataBuffer->getFormat() == RH_PIX_FORMAT_I420 ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_RGBA;

                AVFrame * pReturnFrame = av_frame_alloc();
                pReturnFrame->width = pOutDataBuffer->getWidth();
                pReturnFrame->height = pOutDataBuffer->getHeight();
                pReturnFrame->format = format;

                RHDataSwap pBuffer = {};
                bool bCopyState = pOutDataBuffer->copyData(&pBuffer);

                if(bCopyState) {
                    int ret = av_image_fill_arrays(pReturnFrame->data, pReturnFrame->linesize,
                                                   (const uint8_t *)pBuffer.getData(),
                                                   (AVPixelFormat)format,
                                                   pReturnFrame->width, pReturnFrame->height, 1);
                    if(ret < 0) {
                        SAFE_FREE_AVFRAME(pInFrame)
                        SAFE_FREE_AVFRAME(pReturnFrame)
                        goto AGAIN;
                    }
                }

                if(mFunc) {
                    mFunc(pReturnFrame);
                }
                SAFE_FREE_AVFRAME(pReturnFrame)
            }
            SAFE_FREE_AVFRAME(pInFrame)
        }
    }
    END:
    ALOGI("frame handle thread end.");
}

void RHImageConvertThread::wait() {
    SAFE_WAIT_THREAD(mImageMutex, mImageCond)
}

void RHImageConvertThread::notify() {
    SAFE_NOTIFY_THREAD(mImageMutex, mImageCond)
}
