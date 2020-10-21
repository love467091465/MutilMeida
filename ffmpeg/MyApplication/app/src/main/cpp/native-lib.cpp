#include <jni.h>
#include <string>
#include "FFmpegPlayDemo.h"
#include <android/log.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

#define TAG "myDemo-jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型
char *jstringTostring(JNIEnv *env, jstring jstr);

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_kass_MainActivity_render(
        JNIEnv *env,
        jobject thiz, jobject surface, jstring inputfile) {
    LOGD("ENTER render jni");
    FFmpegPlayDemo *fFmpegPlayDemo = new FFmpegPlayDemo();
    const char *cInputfile = jstringTostring(env, inputfile);
    fFmpegPlayDemo->init(inputfile, env, surface);
    return env->NewStringUTF("HELLO FFMPEG");\

}

char *jstringTostring(JNIEnv *env, jstring jstr) {
    char *rtn = NULL;
    //jclass clsstring = env->FindClass("java/lang/String");
    jclass clsstring = (env)->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes",
                                     "(Ljava/lang/String;)[B");
    jbyteArray barr = (jbyteArray) env->CallObjectMethod(jstr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte *ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0) {
        rtn = (char *) malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}