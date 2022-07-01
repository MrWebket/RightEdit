//
// Created by Hope liao on 2022/4/25.
//

#ifndef RIGHTEDIT_RHAPPLICATION_H
#define RIGHTEDIT_RHAPPLICATION_H

#include "../common/RHRefObject.h"

class RHThumbnailMgr;
class RHApplication : public RHRefObject {
public:
    RHApplication();
    virtual ~RHApplication();

public:
    void init();
    void destroy();
public:
    RHThumbnailMgr           * mThumbnailMgr;
};

#endif //RIGHTEDIT_RHAPPLICATION_H
