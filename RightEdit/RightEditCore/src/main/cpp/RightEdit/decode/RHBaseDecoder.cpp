//
// Created by Hope liao on 2022/7/1.
//

#include "RHBaseDecoder.h"
#include "../common/RHAppDef.h"
#include "../common/RHCommonDefine.h"

extern "C" {
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavutil/rational.h>
}

RHBaseDecoder::RHBaseDecoder(RHApplication *_app): RAppObject(_app) {
    mStreamIndex = -1;

    mAVFormatContext = nullptr;
    mAVCodec = nullptr;
    mAVCodecContext = nullptr;
    mAVStream = nullptr;
    mDeocdeWorking = false;
    mDecodeThread = nullptr;
}

RHBaseDecoder::~RHBaseDecoder() {
    if (mAVCodecContext) {
        avcodec_close(mAVCodecContext);
        avcodec_free_context(&mAVCodecContext);
    }
    mAVCodecContext = nullptr;
    mAVCodec = nullptr;
    mAVFormatContext = nullptr;
    mAVStream = nullptr;
    mStreamIndex = -1;
    mDeocdeWorking = false;
}


void RHBaseDecoder::start() {
    mDeocdeWorking = true;
    mDecodeThread = new std::thread(&RHBaseDecoder::onDecodeThread, this);
}

void RHBaseDecoder::stop() {
    mDeocdeWorking = false;
    SAFE_DELETE_THREAD(mDecodeThread)
}

void RHBaseDecoder::pushPacket(AVPacket *pInPacket) {
    if(pInPacket) {
        AVPacket * dst = av_packet_clone(pInPacket);
        mPacketCacheSize += dst->size;
        mPackketQueue.push_back(dst);
    }
}

uint64_t RHBaseDecoder::getPacketCacheSize() const {
    return mPacketCacheSize;
}

int RHBaseDecoder::openStream(AVFormatContext *formatContext, int stream) {
    int result = -1;
    AVStream * pStream = formatContext->streams[stream];
    //5.获取解码器参数
    AVCodecParameters *codecParameters = pStream->codecpar;

    AVCodec * pCodec = avcodec_find_decoder(codecParameters->codec_id);

    //7.创建解码器上下文
    AVCodecContext * pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext) {
        ALOGE("Failed to alloc the %s codec context\n",
              av_get_media_type_string((AVMediaType) getMediaType()));
        return RH_ERROR_CREATE_DECODER;
    }

    //将解码相关参数设置给解码器
    result = avcodec_parameters_to_context(pCodecContext, codecParameters);
    if (result != 0) {
        ALOGE("avcodec_parameters_to_context fail result = %d.%s\n", result, av_err2str(result));
        return RH_ERROR_CREATE_DECODER;
    }

    AVDictionary *opts = NULL;
    av_dict_set(&opts, "threads", getThreads(), 0);
    av_dict_set(&opts, "refcounted_frames", "1", 0); //ffmpeg内存管理启用引用计数

    //8.打开解码器
    result = avcodec_open2(pCodecContext, pCodec, &opts);
    if (result != 0) {
        ALOGE(" avcodec_open2 fail. result = %d\n", AVERROR(result));
        return RH_ERROR_CREATE_DECODER;
    }

    if (opts) {
        av_dict_free(&opts);
    }

    mAVCodecContext = pCodecContext;
    mAVCodec = pCodec;
    mAVStream = pStream;
    mStreamIndex = stream;
    return RH_SUCCEED;
}

const char * RHBaseDecoder::getThreads() {
    return "auto";
}

void RHBaseDecoder::onDecodeThread() {
    while (mDeocdeWorking) {

    }
}
