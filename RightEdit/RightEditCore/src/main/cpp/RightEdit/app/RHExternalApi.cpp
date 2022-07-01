//
// Created by Hope liao on 2022/5/8.
//

#include "RHExternalApi.h"
#include "../data/RHThumbnailParam.h"
#include "RHApplication.h"
#include "../thumbnail/RHThumbnailMgr.h"

RHExternalApi::RHExternalApi() {
    mApp = new RHApplication();
    mApp->init();
}

RHExternalApi::~RHExternalApi() noexcept {
    if(mApp) {
        mApp->destroy();
        mApp->release();
        mApp = nullptr;
    }
}

void RHExternalApi::loadThumbnail(RHThumbnailParam *_param) {
    mApp->mThumbnailMgr->loadThumbnail(_param);
}