
#ifndef V6CORESDK_JNIHELPER_H
#define V6CORESDK_JNIHELPER_H

#include <jni.h>
#include <string>


std::string jstring2string(JNIEnv *env, jstring jStr) ;
bool getClassName(JNIEnv * jni, jobject object, std::string &outClass);

inline jlong addr2java(void *p)
{
    return (uintptr_t) p;
}

inline void *java2addr(jlong p)
{
    return (void *) p;
}


#endif //V6CORESDK_JNIHELPER_H
