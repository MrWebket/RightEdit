//
// Created by Hope liao on 2022/5/8.
//

#ifndef RIGHTEDIT_RHEXTERNALAPI_H
#define RIGHTEDIT_RHEXTERNALAPI_H

#include "../common/RHRefObject.h"

class RHThumbnailParam;
class RHApplication;
class RHExternalApi: public RHRefObject {
public:
    RHExternalApi();
    virtual ~RHExternalApi();

public:
    void loadThumbnail(RHThumbnailParam * _param);

private:
    RHApplication * mApp;
};

#endif //RIGHTEDIT_RHEXTERNALAPI_H
