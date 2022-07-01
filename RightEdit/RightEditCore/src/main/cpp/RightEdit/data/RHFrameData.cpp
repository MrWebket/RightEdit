//
// Created by Hope liao on 2022/6/15.
//

#include "RHFrameData.h"
#include <string>
#include "../common/RHCommonDefine.h"
#include "../common/RHAppDef.h"


RHFrameData::RHFrameData() {

    reset();
}

RHFrameData::RHFrameData(int w, int h, int format) {
    alloc(w, h, format);
}

void RHFrameData::setData(void *_data) {
    setData(_data, mWidth, mHeight);
}

void RHFrameData::alloc(int w, int h, int format) {
    mWidth = w;
    mHeight = h;
    mFormat = format;

    switch (mFormat) {
        case RH_PIX_FORMAT_I420:
            mDataY.apply(mWidth * mHeight);
            mDataU.apply(mWidth * mHeight / 4);
            mDataV.apply(mWidth * mHeight / 4);

            mLineSize1 = mWidth;
            mLineSize2 = mWidth / 2;
            mLineSize3 = mWidth / 2;
            break;
        case RH_PIX_FORMAT_NV21:
        case RH_PIX_FORMAT_NV12:
            mDataY.apply(mWidth * mHeight);
            mDataUV.apply(mWidth * mHeight / 2); //8

            mLineSize1 = mWidth;
            mLineSize2 = mWidth;
            break;
        case RH_PIX_FORMAT_RGBA:
            mDataRGBA.apply(mWidth * mHeight * 4);
            mLineSize1 = mWidth * 4;
            break;
    }

}

bool RHFrameData::copy(RHFrameData * _dst) {
    if (mWidth <= 0 || mHeight <= 0) {
        return false;
    }
    _dst->alloc(mWidth, mHeight, mFormat);
    if (mFormat == RH_PIX_FORMAT_I420) {
        memcpy(_dst->getDataY(), getDataY(), mDataY.getSize());
        memcpy(_dst->getDataU(), getDataU(), mDataU.getSize());
        memcpy(_dst->getDataV(), getDataV(), mDataV.getSize());
    } else if (mFormat == RH_PIX_FORMAT_NV21 || mFormat == RH_PIX_FORMAT_NV12) {
        memcpy(_dst->getDataY(), getDataY(), mDataY.getSize());
        memcpy(_dst->getDataUV(), getDataUV(), mDataUV.getSize());
    } else if(mFormat == RH_PIX_FORMAT_RGBA) {
        memcpy(_dst->getDataRGBA(), getDataRGBA(), mDataRGBA.getSize());
    }
    return true;
}

bool RHFrameData::copyData(RHDataSwap * _dst) {
    if (mWidth <= 0 || mHeight <= 0 || !_dst) {
        return false;
    }
    _dst->reback();
    _dst->apply(getByteSizes());
    if(mFormat == RH_PIX_FORMAT_I420) {
        auto pData = (uint8_t *)_dst->getData();
        long offset = 0;
        long length = mDataY.getSize();
        memcpy(pData + offset, mDataY.getData(), length);
        offset += length;
        length = mDataU.getSize();
        memcpy(pData + offset, mDataU.getData(), length);
        offset += length;
        length = mDataV.getSize();
        memcpy(pData + offset, mDataV.getData(), length);
    } else if(mFormat == RH_PIX_FORMAT_NV12 || mFormat == RH_PIX_FORMAT_NV21) {
        auto pData = (uint8_t *)_dst->getData();
        long offset = 0;
        long length = mDataY.getSize();
        memcpy(pData + offset, mDataY.getData(), length);
        offset += length;
        length = mDataUV.getSize();
        memcpy(pData + offset, mDataUV.getData(), length);
    } else if(mFormat == RH_PIX_FORMAT_RGBA) {
        auto pData = (uint8_t *)_dst->getData();
        memcpy(pData, mDataRGBA.getData(), _dst->getSize());
    } else {
        return false;
    }

    return true;
}

void RHFrameData::setFroamt(int format) {
    int oldFormat = mFormat;
    if(oldFormat != format) {
        int oldWidth = mWidth;
        int oldHeight = mHeight;
        reset();
        if(oldWidth != 0 && oldHeight != 0) {
            alloc(oldWidth, oldHeight, format);
        }
    }
}

void RHFrameData::setData(void *_data, int w, int h) {
    if (w == 0 || h == 0) {
        return;
    }
    if (mWidth != w || mHeight != h) {
        reset();
        alloc(w, h, mFormat);
    }
    auto *_inData = (uint8_t *) _data;
    if (mFormat == RH_PIX_FORMAT_I420) {
        memcpy(mDataY.getData(), _inData, (mWidth * mHeight));
        memcpy(mDataU.getData(), _inData + mWidth * mHeight, (size_t) (mWidth * mHeight / 4));
        memcpy(mDataV.getData(), _inData + mWidth * mHeight * 5 / 4, (size_t) (mWidth * mHeight / 4));
    } else if (mFormat == RH_PIX_FORMAT_NV21 || mFormat == RH_PIX_FORMAT_NV12) {
        memcpy(mDataY.getData(), _inData, (mWidth * mHeight));
        memcpy(mDataUV.getData(), _inData, (mWidth * mHeight / 2));
    }else if(mFormat == RH_PIX_FORMAT_RGBA) {
        memcpy(mDataRGBA.getData(), _data, mWidth * mHeight * 4);
    }
}

void RHFrameData::setDataY(void *_data, int size) {
    if(mWidth == 0 || mHeight == 0) {
        return;
    }
    int realBytes = mWidth * mHeight;
    if(realBytes != size) {
        return;
    }
    memcpy(mDataY.getData(), _data, size);
}

void RHFrameData::setDataU(void * _data, int size) {
    if(mWidth == 0 || mHeight == 0) {
        return;
    }
    if(mFormat == RH_PIX_FORMAT_I420) {
        int realBytes = mWidth * mHeight / 4;
        if(realBytes != size) {
            return;
        }
        memcpy(mDataU.getData(), _data, size);
    }
}

void RHFrameData::setDataV(void * _data, int size) {
    if(mWidth == 0 || mHeight == 0) {
        return;
    }
    if(mFormat == RH_PIX_FORMAT_I420) {
        int realBytes = mWidth * mHeight / 4;
        if(realBytes != size) {
            return;
        }
        memcpy(mDataV.getData(), _data, size);
    }
}

void RHFrameData::setRGBA(void * _data, int size) {
    if(mWidth == 0 || mHeight == 0) {
        return;
    }
    if(mFormat == RH_PIX_FORMAT_RGBA) {
        int realBytes = mWidth * mHeight * 4;
        if(realBytes != size) {
            return;
        }
        memcpy(mDataRGBA.getData(), _data, size);
    }
}

int RHFrameData::getByteSizes() {
    if(mFormat == RH_PIX_FORMAT_RGBA) {
        return mWidth * mHeight * 4;
    }
    return mWidth * mHeight * 3 / 2;
}

RHFrameData::~RHFrameData(){
    reset();
}


void RHFrameData::reset() {
    mWidth = mHeight = 0;

    mDataY.reback();
    mDataY.reback();
    mDataU.reback();
    mDataV.reback();
    mDataUV.reback();
    mDataRGBA.reback();

    mLineSize1 = mLineSize2 = mLineSize3 = 0;

    mFormat = RH_PIX_FORMAT_I420;
}
