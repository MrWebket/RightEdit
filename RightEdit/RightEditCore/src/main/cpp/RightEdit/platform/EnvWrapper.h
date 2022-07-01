//
// Created by Hope liao on 2022/6/30.
//

#ifndef RIGHTEDIT_ENVWRAPPER_H
#define RIGHTEDIT_ENVWRAPPER_H
#include <jni.h>

class EnvWrapper {
public:
    EnvWrapper();
    ~EnvWrapper();

    JNIEnv * getEnv();

private:
    JNIEnv * mEnv;
};

#endif //RIGHTEDIT_ENVWRAPPER_H
