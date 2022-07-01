//
// Created by Hope liao on 2022/4/27.
//

#ifndef RIGHTEDIT_RHTHUMBNAILDEF_H
#define RIGHTEDIT_RHTHUMBNAILDEF_H

#include "../common/RHAppDef.h"

typedef struct RThumbnailItem {
    std::string mPath;
    std::thread * mReadThread;
    long mDuration;
    int mGopSize;
    int mFps;
    int mRotate;

    RThumbnailItem() {
        mReadThread = nullptr;
        mGopSize = mFps = mRotate = 0;
        mDuration = 0;
    }

    ~RThumbnailItem() {
        SAFE_DELETE_THREAD(mReadThread)
        mGopSize = mFps = mRotate = 0;
        mDuration = 0;
    }
} RThumbnailItem;

#endif //RIGHTEDIT_RHTHUMBNAILDEF_H
