//
// Created by Hope liao on 2022/6/15.
//

#ifndef RIGHTEDIT_RHFRAMEDATA_H
#define RIGHTEDIT_RHFRAMEDATA_H

#include <cstdint>
#include "../common/RHRefObject.h"
#include "../common/RHDataSwap.h"

class RHFrameData: public RHRefObject {
public:
    RHFrameData();
    RHFrameData(int w, int h, int format);
    ~RHFrameData();

public:
    void setData(void * _data, int w, int h);
    void setDataY(void * _data, int size);
    void setDataU(void * _data, int size);
    void setDataV(void * _data, int size);
    void setRGBA(void * _data, int size);
    void setData(void * _data);
    void setFroamt(int format);
    void alloc(int w, int h, int format);
    bool copy(RHFrameData * dst);
    bool copyData(RHDataSwap * _dst);

    inline int getLinesize0() {
        return mLineSize1;
    }
    inline int getLinesize1() {
        return mLineSize2;
    }
    inline int getLinesize2() {
        return mLineSize3;
    }
    inline uint8_t * getDataY() {
        return (uint8_t *)mDataY.getData();
    }
    inline uint8_t * getDataU() {
        return (uint8_t *)mDataU.getData();
    }
    inline uint8_t * getDataV() {
        return (uint8_t *)mDataV.getData();
    }
    inline uint8_t * getDataUV() {
        return (uint8_t *)mDataUV.getData();
    }
    inline uint8_t * getDataRGBA() {
        return (uint8_t *)mDataRGBA.getData();
    }
    inline int getWidth() {
        return mWidth;
    }
    inline int getHeight() {
        return mHeight;
    }
    inline int getFormat() {
        return mFormat;
    }
    int getByteSizes();
private:
    void reset();
private:
    int             mWidth;
    int             mHeight;
    int             mLineSize1;
    int             mLineSize2;
    int             mLineSize3;
    int             mFormat;
    RHDataSwap      mDataY;
    RHDataSwap      mDataU;
    RHDataSwap      mDataV;
    RHDataSwap      mDataUV;

    RHDataSwap      mDataRGBA;
};

#endif //RIGHTEDIT_RHFRAMEDATA_H
