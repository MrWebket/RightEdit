//
// Created by Hope liao on 2022/4/25.
//

#include "RHApplication.h"
#include "../thumbnail/RHThumbnailMgr.h"

RHApplication::RHApplication() {
    mThumbnailMgr = nullptr;
}

RHApplication::~RHApplication() noexcept {
    mThumbnailMgr = nullptr;
}

void RHApplication::init() {
    mThumbnailMgr = new RHThumbnailMgr(this);
    mThumbnailMgr->init();
}

void RHApplication::destroy() {
    if(mThumbnailMgr) {
        mThumbnailMgr->destroy();
        mThumbnailMgr->release();
    }
    mThumbnailMgr = nullptr;
}