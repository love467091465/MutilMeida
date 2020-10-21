//
// Created by ts on 20-8-28.
//

#ifndef MY_APPLICATION_FFMPEGPLAYDEMO_H
#define MY_APPLICATION_FFMPEGPLAYDEMO_H


extern "C" {
#include <libavcodec/avcodec.h>
#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
}

class FFmpegPlayDemo {
private:
    AVFormatContext *mAVFormatContext;
    AVCodecContext *mAVCodecContext;
public:
    FFmpegPlayDemo();

    void init(jstring inputPath,JNIEnv* env,jobject surface);

    void pgm_save(uint8_t *string, int i, int width, int height);

    FILE *mFile;
};


#endif //MY_APPLICATION_FFMPEGPLAYDEMO_H
