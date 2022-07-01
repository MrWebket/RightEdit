//
// Created by Hope liao on 2022/4/25.
//

#ifndef RIGHTEDIT_RHTHUMBNAILMGR_H
#define RIGHTEDIT_RHTHUMBNAILMGR_H

#include <string>
#include <vector>
#include "../common/RAppObject.h"
#include "../app/RHApplication.h"

#include <string>
#include <thread>
#include <map>


class RHThumbnailParam;
class RHThumbnailDecode2;
class RHThumbnailMgr: public RAppObject {

public:
    RHThumbnailMgr(RHApplication * _app);
    ~RHThumbnailMgr();

public:
    void init();
    void destroy();
    void loadThumbnail(RHThumbnailParam * _param);

private:
private:
    typedef std::map<std::string,RHThumbnailDecode2 *> THUMBNAIL_MAP;
    THUMBNAIL_MAP       mThumbnailMap;
};

#endif //RIGHTEDIT_RHTHUMBNAILMGR_H
