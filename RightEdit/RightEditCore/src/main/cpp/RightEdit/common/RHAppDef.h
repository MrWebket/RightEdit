#ifndef V6CORESDK_V6APPDEF_H
#define V6CORESDK_V6APPDEF_H

#if defined(__ANDROID__)
#include <android/log.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <jni.h>
#include <zconf.h>
#define RH_ANDROID "Android"
#endif

#if defined(__APPLE__) || defined(MACOSX)
#include <GLKit/GLKit.h>
#define RH_IOS "IOS"
#include <pthread.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <Foundation/Foundation.h>
#include <OpenGLES/EAGL.h>
#include <OpenGLES/ES3/gl.h>
#include <OpenGLES/ES3/glext.h>
#endif


#define V6_CORE_VERSION "1.0.0_202201240001"

#define LOG_TAG  "RHCode"

#define DEBUG

#ifdef DEBUG
    #if defined(RH_ANDROID)
        #define  ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
        #define  ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
        #define  ALOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
    #elif defined(RH_IOS)
        #define  ALOGD(...) printf(__VA_ARGS__);
        #define  ALOGI(...) printf(__VA_ARGS__);
        #define  ALOGE(...) printf(__VA_ARGS__);
    #endif
#else
    #define  RLOGD(...)
    #define  RLOGI(...)
    #define  RLOGE(...)
#endif


#define LOCK_GUARD(_mutex) \
std::lock_guard<std::mutex> lockGuard(_mutex);

#define GL_DEFAULT_CODE -1

#define FUNCTION        ALOGI("%s", __PRETTY_FUNCTION__);
#define FUNCTION_LINE   ALOGI("%s,line:%d", __PRETTY_FUNCTION__,__LINE__);

#define SAFE_DELETE_THREAD(threadRef) \
if(threadRef) {\
    threadRef->join();\
    delete threadRef;\
    threadRef = nullptr;\
}

#define SAFE_WAIT_THREAD(stdMutex,condition_variabl)\
SAFE_WAIT_THREAD2(stdMutex, condition_variabl, 33)  \

#define SAFE_WAIT_THREAD2(stdMutex,condition_variable, time)\
std::unique_lock<std::mutex> lock(stdMutex);\
condition_variable.wait_for(lock, std::chrono::milliseconds(time)); \
lock.unlock(); \

#define SAFE_NOTIFY_THREAD(stdMutex, condition_variabl)\
std::unique_lock<std::mutex> unique_lock(stdMutex);\
condition_variabl.notify_all();                        \
unique_lock.unlock(); \

#define SAFE_FREE(p) if(p){free(p); p = NULL;}
#define SAFE_DELETE(p) if(p){ delete p; p = NULL;}
#define SAFE_RELEASE(p) \
if(p) {                 \
    p->release();       \
    p = NULL;\
}
#define SAFE_FREE_AVFRAME(_pInFrame) \
if(_pInFrame) { \
    av_frame_unref(_pInFrame); \
    av_frame_free(&_pInFrame); \
    _pInFrame = nullptr; \
}

#define SAFE_FREE_PACKET(_pInPacket) \
if(_pInPacket) { \
    av_packet_unref(_pInPacket); \
    av_packet_free(&_pInPacket); \
    _pInPacket = nullptr; \
}

static void CheckGLError(const char *_file, const char *_func, int _line) {
    GLenum error = glGetError();
    if (error != GL_NO_ERROR) {
        switch (error) {
            case GL_INVALID_ENUM:
                ALOGE("GL Error: 【GL_INVALID_ENUM】  【%s】 【%s】 【%d 行】 \n", _file, _func, _line);
                break;
            case GL_INVALID_VALUE:
                ALOGE("GL Error: 【GL_INVALID_VALUE】  【%s】 【%s】 【%d 行】 \n", _file, _func, _line);
                break;
            case GL_INVALID_OPERATION:
                ALOGE("GL Error: 【GL_INVALID_OPERATION】  【%s】 【%s】 【%d 行】 \n", _file, _func,
                          _line);
                break;
            case GL_OUT_OF_MEMORY:
                ALOGE("GL Error: 【GL_OUT_OF_MEMORY】  【%s】 【%s】 【%d 行】 \n", _file, _func, _line);
                break;
            case GL_INVALID_FRAMEBUFFER_OPERATION:
                break;
            default:
                ALOGE("GL Error: 【%d】  【%s】 【%s】 【%d 行】 \n", error, _file, _func, _line);
                break;
        }
    }
}

#define  _GL_CHECK_ERROR_ CheckGLError(__FILE__, __PRETTY_FUNCTION__, __LINE__);

#define GL_DEF_CODE  -1

#define JNI_CHECK_ERROR(env) \
if(env->ExceptionCheck()) { \
jthrowable exception = env->ExceptionOccurred(); \
env->ExceptionClear(); \
if(exception != NULL) { \
ALOGE("Discarding pending exceptionto throw:%s,line:%d", __PRETTY_FUNCTION__,__LINE__); \
env->DeleteLocalRef(exception); \
} \
}


#endif //V6CORESDK_V6APPDEF_H