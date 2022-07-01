//
// Created by macbookpro on 8/20/21.
//

#include "RHFrameQueue.h"
#include "../common/RHAppDef.h"

RHFrameQueue::RHFrameQueue() {
    LOCK_GUARD(mMutex)
    mQueue = {};
    mFlushFrame = nullptr;
    mSerial = 0;
}

RHFrameQueue::~RHFrameQueue() {
    LOCK_GUARD(mMutex)

    while (!mQueue.empty()) {
        AVFrame *_pFrame = mQueue.front();
        if (_pFrame && _pFrame != mFlushFrame) {
            av_frame_unref(_pFrame);
            av_frame_free(&_pFrame);
        }
        mQueue.pop_front();
    }
    mQueue.clear();
    if(mFlushFrame) {
        av_frame_unref(mFlushFrame);
        av_frame_free(&mFlushFrame);
        mFlushFrame = NULL;
    }

    ALOGI("Release All Frame cache \n");
}

void RHFrameQueue::push_front(AVFrame *_pInFrame) {
    LOCK_GUARD(mMutex)
    mQueue.push_front(_pInFrame);
}

void RHFrameQueue::push_back(AVFrame *_pInFrame) {
    LOCK_GUARD(mMutex)
    mQueue.push_back(_pInFrame);
}

bool RHFrameQueue::push_back(int serial, AVFrame *_pInFrame) {
    LOCK_GUARD(mMutex)
    if(mSerial == serial) {
        mQueue.push_back(_pInFrame);
        return true;
    } else {
        ALOGI("Serial push back faild curr:%d input:%d \n", mSerial, serial);
    }
    return false;
}

AVFrame *RHFrameQueue::front() {
    LOCK_GUARD(mMutex)
    return mQueue.front();
}

AVFrame *RHFrameQueue::back() {
    LOCK_GUARD(mMutex)
    return mQueue.back();
}

void RHFrameQueue::pop_front() {
    LOCK_GUARD(mMutex)
    if(!mQueue.empty()) {
        AVFrame  * _pFrame = mQueue.front();
        if (_pFrame && _pFrame != mFlushFrame) {
            av_frame_unref(_pFrame);
            av_frame_free(&_pFrame);
        }
        mQueue.pop_front();
    }
}

void RHFrameQueue::pop_back() {
    LOCK_GUARD(mMutex)
    mQueue.pop_back();
}

void RHFrameQueue::read_front_pop(AVFrame **_pInFrame) {
    LOCK_GUARD(mMutex)
    if (!mQueue.empty()) {
        AVFrame *_pCurrFrame = mQueue.front();
        if(_pCurrFrame && _pCurrFrame != mFlushFrame) {
            *_pInFrame = _pCurrFrame;
        } else {
            *_pInFrame = NULL;
        }
        mQueue.pop_front();
    }
}

AVFrame * RHFrameQueue::read_front_pop() {
    LOCK_GUARD(mMutex)
    if (!mQueue.empty()) {
        AVFrame *_pCurrFrame = mQueue.front();
        mQueue.pop_front();
        return _pCurrFrame;
    }
    return NULL;
}

void RHFrameQueue::read_front(AVFrame **_pInFrame) {
    LOCK_GUARD(mMutex)
    if (!mQueue.empty()) {
        AVFrame *_pCurrFrame = mQueue.front();
        *_pInFrame = _pCurrFrame;
    }
}

AVFrame *RHFrameQueue::clone_front() {
    LOCK_GUARD(mMutex)
    if (!mQueue.empty()) {
        AVFrame *_pCurrFrame = mQueue.front();
        if(_pCurrFrame) {
            if(_pCurrFrame == mFlushFrame) {
                return mFlushFrame;
            }
            return av_frame_clone(_pCurrFrame);
        }
        mQueue.pop_front();
        return nullptr;
    }
    return nullptr;
}

int64_t RHFrameQueue::getNextPts() {
    LOCK_GUARD(mMutex)
    if(mQueue.size() >= 2) {
        AVFrame * curr_frame = mQueue.at(1);
        if(curr_frame) {
            return curr_frame->pts;
        }
    }
    return AV_NOPTS_VALUE;
}

int64_t RHFrameQueue::getFrontPts() {
    LOCK_GUARD(mMutex)
    AVFrame *_pFrame = mQueue.front();
    if(_pFrame && _pFrame != mFlushFrame) {
        return _pFrame->pts;
    }
    return 0;
}

int RHFrameQueue::flush() {
    LOCK_GUARD(mMutex)
    while (!mQueue.empty()) {
        AVFrame *_pFrame = mQueue.front();
        if (_pFrame && _pFrame != mFlushFrame) {
            av_frame_unref(_pFrame);
            av_frame_free(&_pFrame);
        }
        mQueue.pop_front();
    }
    mQueue.clear();
    mSerial++;
    mQueue.push_back(mFlushFrame);
    
    return mSerial;
}

void RHFrameQueue::clear() {
    LOCK_GUARD(mMutex)
    while (!mQueue.empty()) {
        AVFrame *_pFrame = mQueue.front();
        if (_pFrame && _pFrame != mFlushFrame) {
            av_frame_unref(_pFrame);
            av_frame_free(&_pFrame);
        }
        mQueue.pop_front();
    }
    mQueue.clear();
}

int RHFrameQueue::size() {
    LOCK_GUARD(mMutex)
    return (int)mQueue.size();
}

bool RHFrameQueue::isFlushFrame(AVFrame *frame) {
    LOCK_GUARD(mMutex)
    return frame == mFlushFrame;
}

bool RHFrameQueue::isFrontFlush() {
    LOCK_GUARD(mMutex)
    AVFrame  * _pFrame = mQueue.front();
    return _pFrame == mFlushFrame;
}

int RHFrameQueue::getSerial() {
    LOCK_GUARD(mMutex)
    return mSerial;
}

bool RHFrameQueue::empty() {
    LOCK_GUARD(mMutex)
    return mQueue.empty();
}
