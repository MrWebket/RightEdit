//
// Created by Hope liao on 2022/6/16.
//

#ifndef RIGHTEDIT_RHPNGENCODER_H
#define RIGHTEDIT_RHPNGENCODER_H

#include "RHRefObject.h"

class AVFrame;
class AVFormatContext;
class RHPNGEncoder: public RHRefObject {
public:
    RHPNGEncoder();
    ~RHPNGEncoder();

public:
    int encodeImage(const char * _path, AVFrame * _pInFrame);


private:

};

#endif //RIGHTEDIT_RHPNGENCODER_H
