//
// Created by Hope liao on 2022/6/14.
//

#ifndef RIGHTEDIT_RHCOMMONDEFINE_H
#define RIGHTEDIT_RHCOMMONDEFINE_H

// 旋转角度枚举
enum RHRotation {
    ROTATE_0,
    ROTATE_90,
    ROTATE_180,
    ROTATE_270
};

 enum RHYUVConvertFormat {
    RH_PIX_FORMAT_I420,
    RH_PIX_FORMAT_NV21,
    RH_PIX_FORMAT_NV12,
    RH_PIX_FORMAT_RGBA
};


#define RH_SUCCEED                         0
#define RH_ERROR_IMAGE_ENCODE              -20000

#define RH_ERROR_CREATE_DECODER            -10000

#endif //RIGHTEDIT_RHCOMMONDEFINE_H
