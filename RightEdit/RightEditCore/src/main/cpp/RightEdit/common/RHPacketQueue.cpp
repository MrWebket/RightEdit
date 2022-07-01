//
// Created by macbookpro on 8/20/21.
//

#include "RHPacketQueue.h"
#include "../common/RHAppDef.h"

RHPacketQueue::RHPacketQueue() {
    mQueue = {};
    mFlushPkt = av_packet_alloc();
    av_init_packet(mFlushPkt);
    mFlushPkt->data = nullptr;
    mFlushPkt->size = 0;
//    mFlushPkt->stream_index = mStreamIndex;
}

RHPacketQueue::~RHPacketQueue() {
    flush();
    if(mFlushPkt) {
        av_packet_unref(mFlushPkt);
        av_packet_free(&mFlushPkt);
    }
    mFlushPkt = NULL;
    ALOGI("Release All Packet cache \n");
}

void RHPacketQueue::setFlushPacket(AVPacket *pkt) {
    mFlushPkt = pkt;
}

void RHPacketQueue::push_front(AVPacket *pkt) {
    LOCK_GUARD(mMutex)
    mQueue.push_front(pkt);
}

void RHPacketQueue::push_back(AVPacket *pkt) {
    LOCK_GUARD(mMutex)
    mQueue.push_back(pkt);
}

AVPacket * RHPacketQueue::front() {
    LOCK_GUARD(mMutex)
    return mQueue.front();
}

AVPacket * RHPacketQueue::back() {
    LOCK_GUARD(mMutex)
    return mQueue.back();
}

void RHPacketQueue::pop_front() {
    LOCK_GUARD(mMutex)
    mQueue.pop_front();
}

void RHPacketQueue::pop_back() {
    LOCK_GUARD(mMutex)
    mQueue.pop_back();
}

void RHPacketQueue::readAndPop(AVPacket **pkt) {
    LOCK_GUARD(mMutex)
    if(!mQueue.empty()) {
        AVPacket * _pCurr = mQueue.front();
        *pkt = _pCurr;
        mQueue.pop_front();
    }
}

void RHPacketQueue::read(AVPacket **pkt) {
    LOCK_GUARD(mMutex)
    if(!mQueue.empty()) {
        AVPacket * _pCurrFrame = mQueue.front();
        *pkt = _pCurrFrame;
    }
}

void RHPacketQueue::flush() {
    LOCK_GUARD(mMutex)
    while (!mQueue.empty()) {
        AVPacket * _packet = mQueue.front();
        if(_packet && _packet != mFlushPkt) {
            av_packet_unref(_packet);
            av_packet_free(&_packet);
        }
        mQueue.pop_front();
    }
    mQueue.clear();
}

void RHPacketQueue::pushFlushPacket() {
//    FUNCTION
    LOCK_GUARD(mMutex)
    mQueue.push_back(mFlushPkt);
}

bool RHPacketQueue::isFlushPacket(AVPacket * packet) {
    return mFlushPkt == packet;
}

int RHPacketQueue::size() {
    LOCK_GUARD(mMutex)
    return (int)mQueue.size();
}

bool RHPacketQueue::empty() {
    LOCK_GUARD(mMutex)
    return mQueue.empty();
}
