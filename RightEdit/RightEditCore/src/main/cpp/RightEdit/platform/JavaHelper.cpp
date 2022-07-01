//
// Created by Hope liao on 2022/6/30.
//

#include <jni.h>
#include "JavaHelper.h"

int bInit = 0;
JavaVM * mJavaJVM = NULL;

void BindJavaJVM(void *env) {
    if (!bInit)
    {
        auto * jniENV = (JNIEnv *) env;
        jniENV->GetJavaVM(&mJavaJVM);
        bInit = 1;
    }
}

void BindJavaVM(void *vm) {
    if (!bInit)
    {
        mJavaJVM = (JavaVM *)vm;
        bInit = 1;
    }
}

JNIEnv * GetEnv() {
    if(bInit && mJavaJVM) {
        JNIEnv *env = NULL;
        mJavaJVM->GetEnv((void **) &env, JNI_VERSION_1_6);
        if (env)
            return env;
        else
        {
            int result = mJavaJVM->AttachCurrentThread(&env, NULL);
            if (result != JNI_OK)
            {
                return NULL;
            }
            return env;
        }
    }
    return NULL;
}

void DetachThreadEnv() {
    if(bInit && mJavaJVM) {
        JNIEnv *env = NULL;
        mJavaJVM->GetEnv((void **) &env, JNI_VERSION_1_6);
        if (env){
            mJavaJVM->DetachCurrentThread();
        }
    }
}