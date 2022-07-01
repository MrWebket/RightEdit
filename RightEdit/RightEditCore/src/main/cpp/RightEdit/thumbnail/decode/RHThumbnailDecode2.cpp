//
// Created by Hope liao on 2022/5/4.
//

#include "../../utils/utils.h"
#include "../../common/RHYUVConvert.h"
#include "RHThumbnailDecode2.h"
#include "../../common/RHAppDef.h"
#include "../../data/RHThumbnailParam.h"
#include "../../data/RHFrameData.h"
#include "../../common/RHPNGEncoder.h"
#include "../../basesys/RHImageConvertThread.h"
#include "../../basesys/RHImageEncoderThread.h"

extern "C" {
#include <libavutil/imgutils.h>
}

RHThumbnailDecode2::RHThumbnailDecode2(RHApplication *_app): RAppObject(_app) {
    mDemux = nullptr;
    mCodec = nullptr;
    mCodecContext = nullptr;
    mHwDeviceContext = nullptr;

    mVideoFps = 0;
    mVideoWidth = 0;
    mVideoHeight = 0;
    mGopSize = 0;
    mVideoDuration = 0;
    mVideoRotate = 0;

    mDecoderThread = nullptr;
    mAbortRequest = false;
    mPauseOn = false;
    mThumbnailParam = {};

    mPacketCacheSize = 0;
    mCacheSize = 0;
    mVideoStream = nullptr;
    mVideoRate = {};

    mImageConvertThread = nullptr;
    mImageEncodeThread = nullptr;
    mSupportHWDevice = false;

    mDecodeLock.create();
}

RHThumbnailDecode2::~RHThumbnailDecode2() noexcept {
    if(mDemux) {
        mDemux->stop();
        mDemux->release();
    }
    mDemux = nullptr;
    mCodec = nullptr;
    mCodecContext = nullptr;
    mHwDeviceContext = nullptr;
    mVideoStream = nullptr;
    mVideoRate = {};
    mVideoFps = 0;
    mVideoWidth = 0;
    mVideoHeight = 0;
    mGopSize = 0;
    mVideoDuration = 0;
    mVideoRotate = 0;

    mAbortRequest = mPauseOn  = false;
    mThumbnailParam = {};

    mSupportHWDevice = false;
    mPacketCacheSize = 0;

    if(mImageConvertThread) {
        mImageConvertThread->stop();
        mImageConvertThread->release();
        mImageConvertThread = nullptr;
    }
    if(mImageEncodeThread) {
        mImageEncodeThread->stop();
        mImageEncodeThread->release();
        mImageEncodeThread = nullptr;
    }
    mDecodeLock.destroy();
}

bool RHThumbnailDecode2::onPrepare(AVFormatContext *formatContext, long duration, int videoStreamIndex, int audioStream) {
    int result = -1;
    AVStream * videoStream = formatContext->streams[videoStreamIndex];
    //5.获取解码器参数
    AVCodecParameters *codecParameters = videoStream->codecpar;

    //6.获取解码器
    AVHWDeviceType targetHWType = AV_HWDEVICE_TYPE_NONE;
#if defined(RH_ANDROID)
    targetHWType = AV_HWDEVICE_TYPE_MEDIACODEC;
#elif defined(RH_IOS)
    targetHWType = AV_HWDEVICE_TYPE_VIDEOTOOLBOX;
#endif

    bool bSupportHWDevice = false;

    const char * codecName = av_hwdevice_get_type_name(targetHWType);
    if(!codecName || strcmp(codecName, "") == 0) {
        mCodec = avcodec_find_decoder(codecParameters->codec_id); //软解
        result = 0;
    } else {
        int realType = av_hwdevice_find_type_by_name(codecName);
        if (realType != targetHWType) {
            ALOGI("%s: Not find hardware codec. \n",__func__);
            realType = AV_HWDEVICE_TYPE_NONE;
            mCodec = avcodec_find_decoder(codecParameters->codec_id); //软解
            bSupportHWDevice = false;
        } else {
            if(videoStream->codecpar->codec_id == AV_CODEC_ID_H264) {
                mCodec = avcodec_find_decoder_by_name("h264_mediacodec");
                bSupportHWDevice = true;
                if(mCodec) {
                    result = 0;
                }
            } else if(videoStream->codecpar->codec_id == AV_CODEC_ID_HEVC) {
                mCodec = avcodec_find_decoder_by_name("hevc_mediacodec");
                if(mCodec) {
                    result = 0;
                }
                bSupportHWDevice = true;
            } else {
                result = av_find_best_stream(formatContext, AVMEDIA_TYPE_VIDEO, -1, -1, &mCodec, 0);
                if(result >= 0) {
                    bSupportHWDevice = true;
                }
            }
        }
    }

    if(!mCodec || result < 0) {
        ALOGE("create decode faild \n");
        return false;
    }

    //7.创建解码器上下文
    mCodecContext = avcodec_alloc_context3(mCodec);
    if (!mCodecContext) {
        ALOGE("Failed to alloc the %s codec context\n",
              av_get_media_type_string(AVMEDIA_TYPE_VIDEO));
        return false;
    }

    //将解码相关参数设置给解码器
    result = avcodec_parameters_to_context(mCodecContext, codecParameters);
    // 设置时钟基准
    av_codec_set_pkt_timebase(mCodecContext, videoStream->time_base);

    if(bSupportHWDevice) {
        result = av_hwdevice_ctx_create(&mHwDeviceContext, targetHWType, NULL, NULL, 0); //创建硬解码设备
        if (result >= 0) {
            mCodecContext->hw_device_ctx = av_buffer_ref(mHwDeviceContext);
            ALOGI("create decoder by hwdevice \n");

            for (int i = 0;; i++) {
                const AVCodecHWConfig *hwcodec = avcodec_get_hw_config(mCodec, i);
                if (hwcodec == NULL) break;
                // 可能一个解码器对应着多个硬件加速方式，所以这里将其挑选出来
                if (hwcodec->methods & AV_CODEC_HW_CONFIG_METHOD_HW_DEVICE_CTX && hwcodec->device_type == targetHWType) {
                    mHWPixelFormat = hwcodec->pix_fmt;
                }
            }
        } else {
            mHwDeviceContext = NULL;
            mCodecContext->hw_device_ctx = NULL;
            ALOGI("Failed to create specified HW device.\n");
        }
    }

    if (result != 0) {
        ALOGE("avcodec_parameters_to_context fail result = %d.%s\n", result, av_err2str(result));
        return false;
    }

    mCodecContext->codec_id = mCodec->id;

    AVDictionary *opts = nullptr;
    av_dict_set(&opts, "threads", "auto", 0);
    av_dict_set(&opts, "refcounted_frames", "1", 0); //ffmpeg内存管理启用引用计数

    //8.打开解码器
    result = avcodec_open2(mCodecContext, mCodec, &opts);
    if (result != 0) {
        ALOGE(" avcodec_open2 fail. result = %d\n", result);
        return false;
    }
    if (opts) {
        av_dict_free(&opts);
    }

    AVRational rational = formatContext->streams[videoStreamIndex]->avg_frame_rate;
    mVideoFps = (int)(av_q2d(rational) + 0.5F);
    ALOGI("video fps = %d \n", (int) mVideoFps);

    mGopSize = mCodecContext->gop_size;
    ALOGI("video gopSize = %d \n", mGopSize);

    mVideoWidth = mCodecContext->width;
    mVideoHeight = mCodecContext->height;
    ALOGI("video w = %d, h = %d \n", mVideoWidth, mVideoHeight);

    // 旋转角度
    int rotate = 0;
    AVDictionaryEntry *entry = av_dict_get(videoStream->metadata, "rotate", NULL, AV_DICT_MATCH_CASE);
    if (entry && entry->value) {
        rotate = atoi(entry->value);
    }
    ALOGI("video rotate = %d \n", rotate);
    mVideoRotate = rotate;

    mVideoDuration = duration;
    mVideoStream = videoStream;
    mSupportHWDevice = bSupportHWDevice;

    mVideoRate = av_guess_frame_rate(formatContext, videoStream, NULL);
    if (!mVideoRate.den || !mVideoRate.num) {
        mVideoRate = {1, mVideoFps};
    }

    //关闭非关键帧的环路滤波，seek 大GOP要快100ms左右
//    mCodecContext->skip_loop_filter = AVDISCARD_NONKEY;

    mDecoderThread = new std::thread(&RHThumbnailDecode2::decodeThread, this);
    return true;
}

long RHThumbnailDecode2::getCacheSize() {
    return mPacketCacheSize;
}

void RHThumbnailDecode2::pauseOn(bool pause) {
    mPauseOn = pause;
}

bool RHThumbnailDecode2::start(RHThumbnailParam * _param) {
    if(_param && !_param->verify()) {
        return false;
    }
    mThumbnailParam = *_param;

    if(mDemux) {
        mDemux->stop();
        mDemux->release();
        mDemux = nullptr;
    }
    mDemux = new RHMediaDemux(mApp);
    mDemux->enableStream(true, false);
    mDemux->setDelegate(this);
    mDemux->setDemuxLock(&mDecodeLock);
    mDemux->loadModel(_param->mVideoPath.c_str());
    mDemux->start();
    return true;
}

void RHThumbnailDecode2::stop() {
    mAbortRequest = true;
    if(mDemux) {
        mDemux->stop();
        mDemux->release();
        mDemux = nullptr;
    }
}

void RHThumbnailDecode2::onFlush() {
    mPackketQueue.pushFlushPacket();
}

bool RHThumbnailDecode2::isPause() {
    return mPauseOn;
}

bool RHThumbnailDecode2::isEof() {
    return mImageConvertThread->getSize() == 0 && mPackketQueue.size() == 0;
}

void RHThumbnailDecode2::onDemuxDestroy() {
    mAbortRequest = true;
    SAFE_DELETE_THREAD(mDecoderThread)
}

void RHThumbnailDecode2::onDemuxComplete() {
    ALOGI("%s", __PRETTY_FUNCTION__ );
}

void RHThumbnailDecode2::onError(int errorCode) {
    ALOGE("%s, errorcode:%d",__PRETTY_FUNCTION__,errorCode);
}

void RHThumbnailDecode2::onPacket(int mediaType, AVPacket * inPacket) {
    if(mediaType == AVMEDIA_TYPE_VIDEO) {
        AVPacket * dst = av_packet_clone(inPacket);
        mPackketQueue.push_back(dst);
        mPacketCacheSize += dst->size;
        notifyDecodeThread();
    }
}

void RHThumbnailDecode2::decodeThread() {
    AVFrame * pDecodeFrame = av_frame_alloc();
    AVFrame * pTransferFrame = av_frame_alloc();

    mImageConvertThread = new RHImageConvertThread();
    mImageConvertThread->setCropSize((float) mVideoWidth / (float)mThumbnailParam.mCropWidth, (float)mVideoHeight / (float)mThumbnailParam.mCropHeight);
    mImageConvertThread->setRotation(GetRotation(mVideoRotate));
    mImageConvertThread->setConvertFrameCallback([this](AVFrame * _pInFrame) -> void {
        RHThumbnailDecode2::convertFrameCallback(_pInFrame);
    });
    mImageConvertThread->start();

    bool enableOutputFile = mThumbnailParam.mEnableOutputToFile;
    if(enableOutputFile) {
        mImageEncodeThread = new RHImageEncoderThread();
        mImageEncodeThread->setEncoderPath(mThumbnailParam.mOutputDir.c_str());
        mImageEncodeThread->start();
    }
    bool bFirstRender = true;
    while (true) {
        AGAIN:
        if(mAbortRequest) {
            goto END;
        }

        if (mPackketQueue.empty() || mPauseOn || !mVideoStream) {
            waitDecodeThread();
            goto AGAIN;
        }

        AVPacket * packet = nullptr;
        mPackketQueue.readAndPop(&packet);
        if (!packet) {
            goto AGAIN;
        }

        mPacketCacheSize -= packet->size;
        mPacketCacheSize = std::max((int)mPacketCacheSize, 0);
        int send = AVERROR(EAGAIN);

        mDecodeLock.lock();
        send = avcodec_send_packet(mCodecContext, packet); //将packer数据发送至解码队列
        if(send == AVERROR(EAGAIN)) {
            ALOGI("AVERROR(EAGAIN) pkt:%ld", packet->pts);
            int k = 3;
        }
        ALOGI("packet pts:%ld,dts:%ld，ret:%d", packet->pts,packet->dts, send);
        mDecodeLock.unlock();

        if (send < 0 &&
                send != AVERROR(EAGAIN) &&
                send != AVERROR_EOF) {
            if(!mPackketQueue.isFlushPacket(packet)) {
                ALOGE("SAFE_FREE_PACKET step1");
                SAFE_FREE_PACKET(packet);
            }
            goto AGAIN;
        }

        int receive = AVERROR(EAGAIN);
        do {
            mDecodeLock.lock();
            receive = avcodec_receive_frame(mCodecContext, pDecodeFrame);
            mDecodeLock.unlock();
            if (receive == AVERROR_EOF) {
                mDecodeLock.lock();
                avcodec_flush_buffers(mCodecContext);
                mDecodeLock.unlock();
                av_frame_unref(pDecodeFrame);
                ALOGI("%s AVERROR_EOF\n", __PRETTY_FUNCTION__);
            } else if (receive == AVERROR(EAGAIN)) {
                av_frame_unref(pDecodeFrame);
            } else if (receive == 0) {
                bFirstRender = false;
                if(mVideoStream->codecpar->codec_type == AVMEDIA_TYPE_VIDEO && mHwDeviceContext) { // 硬解
                    if (pDecodeFrame->format == mHWPixelFormat) {
                        if((receive = av_hwframe_transfer_data(pTransferFrame, pDecodeFrame, 0)) < 0) {
                            av_frame_unref(pDecodeFrame);
                            ALOGE("av_hwframe_transfer_data:%d, msg:%d", receive, AVERROR(receive));
                            goto AGAIN;
                        }
                        pushFrameToCache(pTransferFrame);
                        av_frame_unref(pTransferFrame);
                    } else {
                        pushFrameToCache(pDecodeFrame);
                    }
                } else { //软解
                    pushFrameToCache(pDecodeFrame);
                }
                av_frame_unref(pDecodeFrame); //每次读取一帧就释放内存
            } else {
                av_frame_unref(pDecodeFrame);
                ALOGE("%s avcodec_receive_frame faild:%s \n", __PRETTY_FUNCTION__,
                      av_err2str(receive));
            }

        } while (receive != AVERROR(EAGAIN));

        if(mPackketQueue.isFlushPacket(packet)) {
            mDecodeLock.lock();
            avcodec_flush_buffers(mCodecContext);
            mDecodeLock.unlock();
        } else {
            if(send == AVERROR(EAGAIN) && mSupportHWDevice && bFirstRender) {
                mPackketQueue.push_front(packet);
                goto AGAIN;
            }
            SAFE_FREE_PACKET(packet);
        }
    }
    END:
    SAFE_FREE_AVFRAME(pTransferFrame)
    SAFE_FREE_AVFRAME(pDecodeFrame)

    mImageConvertThread->stop();
    mImageConvertThread->release();
    mImageConvertThread = nullptr;

    mImageEncodeThread->stop();
    mImageEncodeThread->release();
    mImageEncodeThread = nullptr;

    if (mCodecContext) {
        if(mCodecContext->hw_device_ctx) {
            av_buffer_unref(&mCodecContext->hw_device_ctx);
            mCodecContext->hw_device_ctx = NULL;
        }
        avcodec_close(mCodecContext);
        avcodec_free_context(&mCodecContext);
    }
    if(mHwDeviceContext) {
        av_buffer_unref(&mHwDeviceContext);
        mHwDeviceContext = NULL;
    }
    mCodec = NULL;
}

void RHThumbnailDecode2::convertFrameCallback(AVFrame * _pInFrame) {
    if(mImageEncodeThread
        && mThumbnailParam.mEnableOutputToFile) {
        mImageEncodeThread->pushFrame(_pInFrame);
    }
}

void RHThumbnailDecode2::pushFrameToCache(AVFrame * pInFrame) {
    uint32_t pts = mCacheSize * mThumbnailParam.mInterval;
    double videoPts = pInFrame->pts * av_q2d(mVideoStream->time_base) * 1000;

    if(videoPts >= static_cast<double>(pts)) {
        mCacheSize++;
//        ALOGE("videoPts:%f, pts:%d", (float)videoPts, pts);
        mImageConvertThread->pushFrame(pInFrame);
    } else {

    }
    ALOGE("old:%ld, videoPts:%lf", pInFrame->pts, videoPts);
}

void RHThumbnailDecode2::waitDecodeThread() {
    SAFE_WAIT_THREAD(mDecodeMutex, mDecodeCond)
}

void RHThumbnailDecode2::notifyDecodeThread() {
    SAFE_NOTIFY_THREAD(mDecodeMutex, mDecodeCond)
}

