//
// Created by Hope liao on 2022/6/14.
//


#include "RHYUVConvert.h"
#include "../common/RHAppDef.h"
#include <cmath>
#include "../data/RHFrameData.h"

extern "C" {
#include <libavutil/pixfmt.h>
#include <libavutil/imgutils.h>
#include <libavutil/frame.h>
}

#include <rotate.h>
#include <convert.h>
#include <scale.h>
#include <video_common.h>

RHYUVConvert::RHYUVConvert() {
    mTargetFormat = RH_PIX_FORMAT_I420;
    mScale = 1.0;
    mConvertData = mScaleData = mRotationData = mRGBAData = nullptr;
    mRotate = ROTATE_0;
}

RHYUVConvert::~RHYUVConvert() {
    mTargetFormat = RH_PIX_FORMAT_I420;
    if(mConvertData) {
        mConvertData->release();
        mConvertData = nullptr;
    }
    if(mScaleData) {
        mScaleData->release();
        mScaleData = nullptr;
    }
    if(mRotationData) {
        mRotationData->release();
        mRotationData = nullptr;
    }
    if(mRGBAData) {
        mRGBAData->release();
        mRGBAData = nullptr;
    }
    mScale = 1.0;
    mRotate = ROTATE_0;
}

void RHYUVConvert::setTargetFormat(RHYUVConvertFormat format) {
    mTargetFormat = format;
}

void RHYUVConvert::setScale(float scale) {
    mScale = scale;
    ALOGI("image convert scale:%f", scale);
}

void RHYUVConvert::setRotation(RHRotation rotation) {
    mRotate = rotation;
}

bool RHYUVConvert::convertFrame(AVFrame *_pInFrame, RHFrameData ** _pOutFrame) {
    RHFrameData * pReturnFrame = NULL;
    bool bNeedRotation = true;
    if(_pInFrame->format == AV_PIX_FMT_NV12 || _pInFrame->format == AV_PIX_FMT_NV21) {
        int realWidth = _pInFrame->width;
        int realHeight = _pInFrame->height;
        if(mRotate == ROTATE_90 || mRotate == ROTATE_270) {
            realWidth = _pInFrame->height;
            realHeight = _pInFrame->width;
        }

        if(mConvertData != nullptr && (mConvertData->getWidth() != realWidth || mConvertData->getHeight() != realHeight)) {
            mConvertData->release();
            mConvertData = NULL;
        }

        if(!mConvertData) {
            mConvertData = new RHFrameData(realWidth, realHeight, RH_PIX_FORMAT_I420);
        }

        int realFrameWidth = _pInFrame->linesize[0];

        RHDataSwap yuvI420SP = {};
        yuvI420SP.apply(realFrameWidth * _pInFrame->height * 3 / 2);
        auto _p420sp = (uint8_t *) yuvI420SP.getData();
        memset(_p420sp, 0, yuvI420SP.getSize());
        int offset = 0;
        int length = realFrameWidth * _pInFrame->height;
        memcpy(_p420sp, _pInFrame->data[0], length);
        offset += length;
        length = realFrameWidth * _pInFrame->height / 2;
        memcpy(_p420sp + offset, _pInFrame->data[1], length);

        libyuv::RotationMode mode;
        switch (mRotate) {
            case ROTATE_0:
                mode = libyuv::kRotate0;
                break;
            case ROTATE_90:
                mode = libyuv::kRotate90;
                break;
            case ROTATE_180:
                mode = libyuv::kRotate180;
                break;
            case ROTATE_270:
                mode = libyuv::kRotate270;
                break;
        }

        libyuv::FourCC yuvFormat = libyuv::FOURCC_ANY;
        if (_pInFrame->format == AV_PIX_FMT_NV12) {
            yuvFormat = libyuv::FOURCC_NV12;
        } else if (_pInFrame->format == AV_PIX_FMT_NV21) {
            yuvFormat = libyuv::FOURCC_NV21;
        }

        int ret = libyuv::ConvertToI420(_p420sp, yuvI420SP.getSize(),
                                        mConvertData->getDataY(), mConvertData->getLinesize0(),
                                        mConvertData->getDataU(), mConvertData->getLinesize1(),
                                        mConvertData->getDataV(), mConvertData->getLinesize2(),
                              0,0,
                                        realFrameWidth, _pInFrame->height,
                              _pInFrame->width, _pInFrame->height,
                              mode, yuvFormat);

        if (ret < 0) {
            ALOGE("Failed to call NV12ToI420: %d", ret);
            return false;
        }
        pReturnFrame = mConvertData;
        bNeedRotation = false;
    } else if(_pInFrame->format == AV_PIX_FMT_YUV420P) {
        if(mConvertData != nullptr && (mConvertData->getByteSizes() != (_pInFrame->width * _pInFrame->height * 3 / 2))) {
            mConvertData->release();
            mConvertData = NULL;
        }

        if(!mConvertData) {
            mConvertData = new RHFrameData(_pInFrame->width, _pInFrame->height, RH_PIX_FORMAT_I420);
        }

        if(_pInFrame->width != _pInFrame->linesize[0]) {
            libyuv::I420Scale(_pInFrame->data[0], _pInFrame->linesize[0],
                              _pInFrame->data[1], _pInFrame->linesize[1],
                              _pInFrame->data[2], _pInFrame->linesize[2],
                              _pInFrame->linesize[0], _pInFrame->height,
                              mConvertData->getDataY(), mConvertData->getLinesize0(),
                              mConvertData->getDataU(), mConvertData->getLinesize1(),
                              mConvertData->getDataV(), mConvertData->getLinesize2(),
                              mConvertData->getWidth(), mConvertData->getHeight(),
                              libyuv::kFilterNone);
        } else {
            mConvertData->setDataY(_pInFrame->data[0], _pInFrame->width * _pInFrame->height);
            mConvertData->setDataU(_pInFrame->data[1], _pInFrame->width * _pInFrame->height / 4);
            mConvertData->setDataV(_pInFrame->data[2], _pInFrame->width * _pInFrame->height / 4);
        }
        pReturnFrame = mConvertData;
        bNeedRotation = true;
    }

    if(mScale > 1.0) {
        int scaleWidth = (int)(pReturnFrame->getWidth() / mScale + 0.5f);
        if(scaleWidth % 2 == 1) {
            scaleWidth += 1;
        }
        int scaleHeight = (int) (pReturnFrame->getHeight() / mScale + 0.5f);
        if(scaleHeight % 2 == 1) {
            scaleHeight += 1;
        }
        int scaleByteSize = scaleWidth * scaleHeight * 3 / 2;

        if(mScaleData != nullptr && (mScaleData->getByteSizes() != (scaleByteSize))) {
            mScaleData->release();
            mScaleData = NULL;
        }

        if(!mScaleData) {
            mScaleData = new RHFrameData(scaleWidth, scaleHeight, RH_PIX_FORMAT_I420);
        }

       int ret = libyuv::I420Scale(pReturnFrame->getDataY(), pReturnFrame->getLinesize0(),
                                   pReturnFrame->getDataU(), pReturnFrame->getLinesize1(),
                                   pReturnFrame->getDataV(), pReturnFrame->getLinesize2(),
                                   pReturnFrame->getWidth(), pReturnFrame->getHeight(),
                                   mScaleData->getDataY(), mScaleData->getLinesize0(),
                                   mScaleData->getDataU(), mScaleData->getLinesize1(),
                                   mScaleData->getDataV(), mScaleData->getLinesize2(),
                                   mScaleData->getWidth(), mScaleData->getHeight(),
                                libyuv::kFilterNone);
        if (ret < 0) {
            ALOGE("Failed to call I420Scale: %d", ret);
            return false;
        }
        pReturnFrame = mScaleData;
    }
    int returnFrameByteSize = pReturnFrame->getWidth() * pReturnFrame->getHeight() * 3 / 2;

    if(mRotate != ROTATE_0 && bNeedRotation) {
        if(mRotationData != nullptr && mRotationData->getByteSizes() != returnFrameByteSize) {
            mRotationData->release();
            mRotationData = nullptr;
        }

        int tmpWidth = pReturnFrame->getWidth();
        int tmpHeight = pReturnFrame->getHeight();
        if(mRotate == ROTATE_90 || mRotate == ROTATE_270) {
            tmpWidth = pReturnFrame->getHeight();
            tmpHeight = pReturnFrame->getWidth();
        }
        if(!mRotationData) {
            mRotationData = new RHFrameData(tmpWidth, tmpHeight, RH_PIX_FORMAT_I420);
        }

        libyuv::RotationMode mode = libyuv::kRotate0;
        switch (mRotate) {
            case ROTATE_0:
                mode = libyuv::kRotate0;
                break;
            case ROTATE_90:
                mode = libyuv::kRotate90;
                break;
            case ROTATE_180:
                mode = libyuv::kRotate180;
                break;
            case ROTATE_270:
                mode = libyuv::kRotate270;
                break;
        }

        int ret = libyuv::I420Rotate(pReturnFrame->getDataY(), pReturnFrame->getLinesize0(),
                                     pReturnFrame->getDataU(), pReturnFrame->getLinesize1(),
                                     pReturnFrame->getDataV(), pReturnFrame->getLinesize2(),
                                     mRotationData->getDataY(), mRotationData->getLinesize0(),
                                     mRotationData->getDataU(), mRotationData->getLinesize1(),
                                     mRotationData->getDataV(), mRotationData->getLinesize2(),
                                     pReturnFrame->getWidth(), pReturnFrame->getHeight(), mode);

        if (ret < 0) {
            ALOGE("Failed to call I420Rotate: %d", ret);
            return false;
        }
        pReturnFrame = mRotationData;
    }

    if(mTargetFormat == RH_PIX_FORMAT_RGBA) {
        int rgbaByteSize = pReturnFrame->getWidth() * pReturnFrame->getHeight() * 4;

        if(mRGBAData != nullptr && mRGBAData->getByteSizes() != rgbaByteSize) {
            mRGBAData->release();
            mRGBAData = nullptr;
        }
        if(!mRGBAData) {
            mRGBAData = new RHFrameData(pReturnFrame->getWidth(), pReturnFrame->getHeight(), RH_PIX_FORMAT_RGBA);
        }

        int ret = libyuv::I420ToABGR(pReturnFrame->getDataY(), pReturnFrame->getLinesize0(),
                                     pReturnFrame->getDataU(), pReturnFrame->getLinesize1(),
                                     pReturnFrame->getDataV(), pReturnFrame->getLinesize2(),
                                     mRGBAData->getDataRGBA(), mRGBAData->getLinesize0(),
                                     mRGBAData->getWidth(), mRGBAData->getHeight());

        if (ret < 0) {
            ALOGE("Failed to call I420ToABGR: %d", ret);
            return false;
        }
        pReturnFrame = mRGBAData;
    }

    *_pOutFrame = pReturnFrame;
    return pReturnFrame;
}
