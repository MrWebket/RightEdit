//
// Created by Hope liao on 2022/6/30.
//

#include "EnvWrapper.h"
#include "JavaHelper.h"


EnvWrapper::EnvWrapper() {
    mEnv = GetEnv();
}

EnvWrapper::~EnvWrapper() {
    if(mEnv) {
        DetachThreadEnv();
    }
}

JNIEnv * EnvWrapper::getEnv() {
    return mEnv;
}