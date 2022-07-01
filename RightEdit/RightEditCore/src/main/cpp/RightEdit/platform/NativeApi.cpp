//
// Created by Hope liao on 2022/4/24.
//

#include "../data/RHThumbnailParam.h"
#include "../app/RHExternalApi.h"
#include "NativeApi.h"
#include "../common/RHAppDef.h"
#include "jniHelper.h"
#include "JavaHelper.h"

extern "C" {

JNIEXPORT jstring JNICALL
Java_com_right_edit_core_jni_NativeApi_getVersion(JNIEnv *env, jclass clazz) {
    return env->NewStringUTF(V6_CORE_VERSION);
}

JNIEXPORT jlong JNICALL
Java_com_right_edit_core_jni_NativeApi_createEngine(JNIEnv *env, jobject clazz) {
    auto * mApi = new RHExternalApi();


    return (jlong)mApi;
}

JNIEXPORT void JNICALL
Java_com_right_edit_core_jni_NativeApi_loadThumbnail(JNIEnv *env, jobject clazz, jlong addr,
                                                     jstring video_path, jstring out_dir,
                                                     jint interval, jint cropWidth, jint cropHeight) {
    auto * api = (RHExternalApi *) addr;
    if(api) {
        std::string videoPath = jstring2string(env, video_path);

        RHThumbnailParam param = {};
        param.mVideoPath = videoPath;
        param.mInterval = interval;
        param.mCropWidth = cropWidth;
        param.mCropHeight = cropHeight;
        if(out_dir != NULL) {
            std::string outDir = jstring2string(env, out_dir);
            bool enableOutputToFile = strcmp(outDir.c_str(), "") != 0;
            param.mOutputDir = outDir;
            param.mEnableOutputToFile = enableOutputToFile;
        }
        api->loadThumbnail(&param);
    }
}

#include <libavcodec/jni.h>
#include <libavcodec/version.h>
#include <libavformat/version.h>
#include <libavfilter/version.h>
#include <libswresample/version.h>
#include <libavcodec/avcodec.h>

jint JNI_OnLoad(JavaVM *jvm, void *p) {
    av_jni_set_java_vm((void *) jvm, NULL);
    BindJavaVM(jvm);
    ALOGI("JNI_OnLoad");
    ALOGI("ffmpeg version:%s \n", av_version_info());
    return JNI_VERSION_1_6;
}

}

