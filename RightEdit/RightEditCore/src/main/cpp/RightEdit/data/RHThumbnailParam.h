//
// Created by Hope liao on 2022/5/8.
//

#ifndef RIGHTEDIT_RHTHUMBNAILPARAM_H
#define RIGHTEDIT_RHTHUMBNAILPARAM_H

#include <string>
#include <vector>

typedef struct RHThumbnailParam{
    std::string mVideoPath;
    std::string mOutputDir;
    int mCropWidth;
    int mCropHeight;
    bool mEnableOutputToFile;
    int  mInterval;

    RHThumbnailParam() {
       reset();
    }

    ~RHThumbnailParam() {
        reset();
    }

    void reset() {
        mOutputDir = mVideoPath = "";
        mEnableOutputToFile = false;
        mInterval = 0;
        mCropWidth = 0;
        mCropHeight = 0;
    }

    bool verify() {
        if(strcmp(mVideoPath.c_str(), "") == 0 || mInterval == 0) {
            return false;
        }
        return true;
    }
} RHThumbnailParam;

#endif //RIGHTEDIT_RHTHUMBNAILPARAM_H
