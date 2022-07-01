//
// Created by Hope liao on 2022/4/24.
//

#ifndef RIGHTEDIT_NATIVEAPI_H
#define RIGHTEDIT_NATIVEAPI_H
#include <jni.h>

extern "C" {
JNIEXPORT jstring JNICALL
Java_com_right_edit_core_jni_NativeApi_getVersion(JNIEnv *env, jclass clazz);

JNIEXPORT jlong JNICALL
Java_com_right_edit_core_jni_NativeApi_createEngine(JNIEnv *env, jobject clazz);

JNIEXPORT void JNICALL
Java_com_right_edit_core_jni_NativeApi_loadThumbnail(JNIEnv *env, jobject clazz, jlong addr,
                                                     jstring video_path, jstring out_dir,
                                                     jint interval, jint cropWidth, jint cropHeight);

};

#endif //RIGHTEDIT_NATIVEAPI_H
