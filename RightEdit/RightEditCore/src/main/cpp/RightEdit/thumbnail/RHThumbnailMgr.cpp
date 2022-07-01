//
// Created by Hope liao on 2022/4/25.
//

#include "RHThumbnailMgr.h"
#include "../common/RHAppDef.h"
#include "thumbnail/decode/RHThumbnailDecode2.h"
#include "../data/RHThumbnailParam.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavutil/rational.h>
}


RHThumbnailMgr::RHThumbnailMgr(RHApplication *_app) : RAppObject(_app){
    mThumbnailMap.clear();
}

RHThumbnailMgr::~RHThumbnailMgr() noexcept {
    mThumbnailMap.clear();
}

void RHThumbnailMgr::init() {

}

void RHThumbnailMgr::destroy() {

}

void RHThumbnailMgr::loadThumbnail(RHThumbnailParam * _param) {
    const char * _path = _param->mVideoPath.c_str();

    if(mThumbnailMap.find(_path) != mThumbnailMap.end()) {
        ALOGE("exists target thumbnail task:%s\n", _path);
        return;
    }
    auto * pDecoder = new RHThumbnailDecode2(mApp);
    mThumbnailMap.insert(std::make_pair(_path, pDecoder));

    if(!pDecoder->start(_param)) {
        mThumbnailMap.erase(mThumbnailMap.find(_path));
    }

}
