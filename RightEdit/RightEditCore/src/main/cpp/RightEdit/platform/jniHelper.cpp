#include "jniHelper.h"

std::string jstring2string(JNIEnv *env, jstring jStr)
{
    if (!jStr)
        return "";

    const jclass stringClass = env->GetObjectClass(jStr);
    const jmethodID getBytes = env->GetMethodID(stringClass, "getBytes",
                                                "(Ljava/lang/String;)[B");
    const auto stringJbytes = (jbyteArray) env->CallObjectMethod(jStr, getBytes,
                                                                       env->NewStringUTF(
                                                                               "UTF-8"));
    size_t length = (size_t) env->GetArrayLength(stringJbytes);
    jbyte *pBytes = env->GetByteArrayElements(stringJbytes, NULL);

    std::string ret = std::string((char *) pBytes, length);
    env->ReleaseByteArrayElements(stringJbytes, pBytes, JNI_ABORT);

    env->DeleteLocalRef(stringJbytes);
    env->DeleteLocalRef(stringClass);
    return ret;
}
bool getClassName(JNIEnv * env, jobject object, std::string &outClass) {
    if (env) {
        jclass jclass = env->GetObjectClass(object);
        if (jclass == nullptr)
            return false;
        jmethodID mid = env->GetMethodID(jclass, "getClass", "()Ljava/lang/Class;");
        if (mid == nullptr)
            return false;
        jobject clsObj = env->CallObjectMethod(object, mid);
//        env->DeleteGlobalRef(jclass);

        if (clsObj == nullptr)
            return false;
        jclass = env->GetObjectClass(clsObj);
        if (jclass == nullptr)
            return false;
        mid = env->GetMethodID(jclass, "getName", "()Ljava/lang/String;");
        if (mid == nullptr)
            return false;
        auto strObj = (jstring) env->CallObjectMethod(clsObj, mid);
        const char *className = env->GetStringUTFChars(strObj, 0);
        outClass = className;

//        env->DeleteGlobalRef(jclass);
        env->ReleaseStringUTFChars(strObj, className);
        return true;
    }
    return false;
}
