//
// Created by Hope liao on 2022/6/14.
//

#ifndef RIGHTEDIT_RHYUVCONVERT_H
#define RIGHTEDIT_RHYUVCONVERT_H

#include "RHRefObject.h"
#include "RHDataSwap.h"
#include "RHCommonDefine.h"



class AVFrame;
class RHFrameData;
class RHYUVConvert: public RHRefObject {
public:
    RHYUVConvert();
    ~RHYUVConvert();

public:
    void setScale(float scale);
    void setRotation(RHRotation rotation);
    void setTargetFormat(RHYUVConvertFormat format);
    bool convertFrame(AVFrame * _pInFrame, RHFrameData ** _pOutFrame);
private:
    RHYUVConvertFormat           mTargetFormat;
    RHRotation                   mRotate;

    RHFrameData                  * mConvertData;
    RHFrameData                  * mScaleData;
    RHFrameData                  * mRotationData;
    RHFrameData                  * mRGBAData;

    float                        mScale;
};

#endif //RIGHTEDIT_RHYUVCONVERT_H
