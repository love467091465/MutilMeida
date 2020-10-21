//
// Created by ts on 20-8-28.
//

#include <jni.h>
#include "FFmpegPlayDemo.h"
#include <android/log.h>
#include <unistd.h>

#include <android/native_window_jni.h>
#include <android/native_window.h>
#include <string>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#define TAG "myDemo-jni" // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,TAG ,__VA_ARGS__) // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,TAG ,__VA_ARGS__) // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,TAG ,__VA_ARGS__) // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,TAG ,__VA_ARGS__) // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL,TAG ,__VA_ARGS__) // 定义LOGF类型
#define INBUF_SIZE 4096
ANativeWindow *mNativeWindow;

FFmpegPlayDemo::FFmpegPlayDemo()
        : mAVFormatContext(NULL),
          mAVCodecContext(NULL) {
};

static void decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt) {
    char buf[1024];
    int ret;
    LOGD("decode one frame");

//视频缓冲区
    ANativeWindow_Buffer native_outBuffer;

    AVFrame *rgb_frame = av_frame_alloc();//分配一个AVFrame结构体，指向存放转换成rgb后的帧


    SwsContext *sContext = sws_getContext(frame->width, frame->height, AV_PIX_FMT_NV12,
                                          frame->width, frame->height, AV_PIX_FMT_ARGB,
                                          0, NULL, NULL, NULL);
    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
            return;
        else if (ret < 0) {
            LOGD("Error during decoding\n");
            return;
        }

        ret = avcodec_send_packet(dec_ctx, pkt);
        if (ret < 0) {
            LOGD("Error sending a packet for decoding\n");
            return;
        }
        LOGD("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);


        ANativeWindow_lock(mNativeWindow, &native_outBuffer, NULL);
        //转换为rgb格式
        sws_scale(sContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                  frame->height, rgb_frame->data,
                  rgb_frame->linesize);

        uint8_t *dst = (uint8_t *) native_outBuffer.bits;
//            拿到一行有多少个字节 RGBA
        int destStride = native_outBuffer.stride * 4;
        //像素数据的首地址
        uint8_t *src = rgb_frame->data[0];
//            实际内存一行数量
        int srcStride = rgb_frame->linesize[0];
        //int i=0;
        for (int i = 0; i < frame->height; ++i) {
//                memcpy(void *dest, const void *src, size_t n)
            //将rgb_frame中每一行的数据复制给nativewindow
            memcpy(dst + i * destStride, src + i * srcStride, srcStride);
        }
        ANativeWindow_unlockAndPost(mNativeWindow);
        usleep(1000 * 16);
    }
}


void FFmpegPlayDemo::init(jstring inputPath, JNIEnv *env, jobject surface) {
    const char *input = env->GetStringUTFChars(inputPath, NULL);
    if (input == NULL) {
        LOGD("字符串转换失败......");
        return;
    }
    av_register_all();
    //注册FFmpeg所有编解码器，以及相关协议。

    LOGE("input:%s",input);
    //分配结构体
    AVFormatContext *formatContext = avformat_alloc_context();
    //打开视频数据源。由于Android 对SDK存储权限的原因，如果没有为当前项目赋予SDK存储权限，打开本地视频文件时会失败
    int open_state = avformat_open_input(&formatContext, input, NULL, NULL);
    if (open_state < 0) {
        char errbuf[128];
        if (av_strerror(open_state, errbuf, sizeof(errbuf)) == 0){
            LOGD("打开视频输入流信息失败，失败原因： %s", errbuf);
        }
        return;
    }
    //为分配的AVFormatContext 结构体中填充数据
    if (avformat_find_stream_info(formatContext, NULL) < 0) {
        LOGD("读取输入的视频流信息失败。");
        return;
    }
    int video_stream_index = -1;//记录视频流所在数组下标
    LOGD("当前视频数据，包含的数据流数量：%d", formatContext->nb_streams);
    //找到"视频流".AVFormatContext 结构体中的nb_streams字段存储的就是当前视频文件中所包含的总数据流数量——
    //视频流，音频流，字幕流
    for (int i = 0; i < formatContext->nb_streams; i++) {

        //如果是数据流的编码格式为AVMEDIA_TYPE_VIDEO——视频流。
        if (formatContext->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;//记录视频流下标
            break;
        }
    }
    if (video_stream_index == -1) {
        LOGD("没有找到 视频流。");
        return;
    }
    //通过编解码器的id——codec_id 获取对应（视频）流解码器
    AVCodecParameters *codecParameters=formatContext->streams[video_stream_index]->codecpar;
    AVCodec *videoDecoder = avcodec_find_decoder(codecParameters->codec_id);

    if (videoDecoder == NULL) {
        LOGD("未找到对应的流解码器。");
        return;
    }
    //通过解码器分配(并用  默认值   初始化)一个解码器context
    AVCodecContext *codecContext = avcodec_alloc_context3(videoDecoder);

    if (codecContext == NULL) {
        LOGD("分配 解码器上下文失败。");
        return;
    }
    //更具指定的编码器值填充编码器上下文
    if(avcodec_parameters_to_context(codecContext,codecParameters)<0){
        LOGD("填充编解码器上下文失败。");
        return;
    }
    //通过所给的编解码器初始化编解码器上下文
    if (avcodec_open2(codecContext, videoDecoder, NULL) < 0) {
        LOGD("初始化 解码器上下文失败。");
        return;
    }
//     mFile = fopen("/storage/emulated/0/test.yuv","ab+");
//    if (!mFile) {
//        LOGD("打开yuv文件失败");
//        return;
//    }

    AVPixelFormat dstFormat = AV_PIX_FMT_RGBA;
    //分配存储压缩数据的结构体对象AVPacket
    //如果是视频流，AVPacket会包含一帧的压缩数据。
    //但如果是音频则可能会包含多帧的压缩数据
    AVPacket *packet = av_packet_alloc();
    //分配解码后的每一数据信息的结构体（指针）
    AVFrame *frame = av_frame_alloc();
    //分配最终显示出来的目标帧信息的结构体（指针）
    AVFrame *outFrame = av_frame_alloc();
    uint8_t *out_buffer = (uint8_t *) av_malloc(
            (size_t) av_image_get_buffer_size(dstFormat, codecContext->width, codecContext->height,
                                              1));
    //更具指定的数据初始化/填充缓冲区
    av_image_fill_arrays(outFrame->data, outFrame->linesize, out_buffer, dstFormat,
                         codecContext->width, codecContext->height, 1);
    //初始化SwsContext
    SwsContext *swsContext = sws_getContext(
            codecContext->width   //原图片的宽
            ,codecContext->height  //源图高
            ,codecContext->pix_fmt //源图片format
            ,codecContext->width  //目标图的宽
            ,codecContext->height  //目标图的高
            ,dstFormat,SWS_BICUBIC
            , NULL, NULL, NULL
    );
    if(swsContext==NULL){
        LOGD("swsContext==NULL");
        return;
    }
    //Android 原生绘制工具
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);
    //定义绘图缓冲区
    ANativeWindow_Buffer outBuffer;
    //通过设置宽高限制缓冲区中的像素数量，而非屏幕的物流显示尺寸。
    //如果缓冲区与物理屏幕的显示尺寸不相符，则实际显示可能会是拉伸，或者被压缩的图像
    ANativeWindow_setBuffersGeometry(nativeWindow, codecContext->width, codecContext->height,
                                     WINDOW_FORMAT_RGBA_8888);
    //循环读取数据流的下一帧
    while (av_read_frame(formatContext, packet) == 0) {
        LOGE("video_stream_index:%d,packet->stream_index :%d",video_stream_index,packet->stream_index );
        if (packet->stream_index == video_stream_index) {
            //讲原始数据发送到解码器
            int sendPacketState = avcodec_send_packet(codecContext, packet);
            if (sendPacketState == 0) {
                int receiveFrameState = avcodec_receive_frame(codecContext, frame);
                if (receiveFrameState == 0) {
                    //锁定窗口绘图界面

//                    LOGE("width,:%d,height:%d,linesize:%d,",codecContext->width,codecContext->height,outFrame->linesize);
//                    for (int i = 0; i < codecContext->height; i++)
//                        fwrite(frame->data[0] + i * frame->linesize[0], 1,  codecContext->width, mFile);
                    ANativeWindow_lock(nativeWindow, &outBuffer, NULL);
                    //对输出图像进行色彩，分辨率缩放，滤波处理
                    sws_scale(swsContext, (const uint8_t *const *) frame->data, frame->linesize, 0,
                              frame->height, outFrame->data, outFrame->linesize);
                    uint8_t *dst = (uint8_t *) outBuffer.bits;
                    //解码后的像素数据首地址
                    //这里由于使用的是RGBA格式，所以解码图像数据只保存在data[0]中。但如果是YUV就会有data[0]
                    //data[1],data[2]
                    uint8_t *src = outFrame->data[0];
                    //获取一行字节数
                    int oneLineByte = outBuffer.stride * 4;
                    //复制一行内存的实际数量
                    int srcStride = outFrame->linesize[0];
                    for (int i = 0; i < codecContext->height; i++) {
                        memcpy(dst + i * oneLineByte, src + i * srcStride, srcStride);
                    }



                    //解锁
                    ANativeWindow_unlockAndPost(nativeWindow);
                    //进行短暂休眠。如果休眠时间太长会导致播放的每帧画面有延迟感，如果短会有加速播放的感觉。
                    //一般一每秒60帧——16毫秒一帧的时间进行休眠
//                    usleep(1000 * 20);//20毫秒

                } else if (receiveFrameState == AVERROR(EAGAIN)) {
                    LOGD("从解码器-接收-数据失败：AVERROR(EAGAIN)");
                } else if (receiveFrameState == AVERROR_EOF) {
                    LOGD("从解码器-接收-数据失败：AVERROR_EOF");
                } else if (receiveFrameState == AVERROR(EINVAL)) {
                    LOGD("从解码器-接收-数据失败：AVERROR(EINVAL)");
                } else {
                    LOGD("从解码器-接收-数据失败：未知");
                }
            } else if (sendPacketState == AVERROR(EAGAIN)) {//发送数据被拒绝，必须尝试先读取数据
                LOGD("向解码器-发送-数据包失败：AVERROR(EAGAIN)");//解码器已经刷新数据但是没有新的数据包能发送给解码器
            } else if (sendPacketState == AVERROR_EOF) {
                LOGD("向解码器-发送-数据失败：AVERROR_EOF");
            } else if (sendPacketState == AVERROR(EINVAL)) {//遍解码器没有打开，或者当前是编码器，也或者需要刷新数据
                LOGD("向解码器-发送-数据失败：AVERROR(EINVAL)");
            } else if (sendPacketState == AVERROR(ENOMEM)) {//数据包无法压如解码器队列，也可能是解码器解码错误
                LOGD("向解码器-发送-数据失败：AVERROR(ENOMEM)");
            } else {
                LOGD("向解码器-发送-数据失败：未知");
            }
        }
        av_packet_unref(packet);
    }
    //内存释放
    ANativeWindow_release(nativeWindow);
    av_frame_free(&outFrame);
    av_frame_free(&frame);
    av_packet_free(&packet);
    avcodec_free_context(&codecContext);
    avformat_close_input(&formatContext);
    avformat_free_context(formatContext);
//    fclose(mFile);
}

void FFmpegPlayDemo::pgm_save(uint8_t *string, int wrap, int xsize, int ysize) {
    LOGE("pgm_save");
    if(mFile==NULL){
        LOGE("yuv 文件创建失败");
        return;
    }
    for (int i = 0; i < ysize; i++)
        fwrite(string + i * wrap, 1, xsize, mFile);

}
