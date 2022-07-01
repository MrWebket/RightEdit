//
// Created by Hope liao on 2022/5/3.
//

#include "../utils/utils.h"
#include "RHMediaDemux.h"
#include "RHAppDef.h"
#include "RErrorCode.h"
#include "RHErrorCode.h"
#include "RHLock.h"

RHMediaDemux::RHMediaDemux(RHApplication *_app): RAppObject(_app) {
    reset();
}

RHMediaDemux::~RHMediaDemux() noexcept {
    reset();
}

void RHMediaDemux::reset() {
    mMediaPath = "";
    mAbortRequest = false;
    mDemuxThread = nullptr;
    mAVFormatContext = nullptr;
    mEnableAudioStream = true;
    mEnableVideoStream = true;
    mDuration = 0;
    mDelegate = nullptr;
    mDemuxLock = nullptr;
    mVideoStreamIndex = mAudioStreamIndex = -1;
    mMaxCacheSize = R_MAX_PACKET_CACHE_SIZE;
    mPauseOn = false;
}

void RHMediaDemux::wait() {
    std::unique_lock<std::mutex> lock(mReadMutex);
    mReadCond.wait_for(lock, std::chrono::milliseconds(33));
    lock.unlock();
}

void RHMediaDemux::notify() {
    std::unique_lock<std::mutex> lock(mReadMutex);
    mReadCond.notify_all();
    mReadMutex.unlock();
}

void RHMediaDemux::loadModel(const char *_path) {
    mMediaPath = _path;
}

void RHMediaDemux::setDelegate(RHMediaDemuxDelegate *_delegate) {
    this->mDelegate = _delegate;
}

void RHMediaDemux::setDemuxLock(RHLock *lock) {
    mDemuxLock = lock;
}

void RHMediaDemux::enableStream(bool enableVideo, bool enableAudio) {
    mEnableAudioStream = enableAudio;
    mEnableVideoStream = enableVideo;
}

int RHMediaDemux::start() {
    if(!mDelegate) {
        return -1;
    }
    mAbortRequest = false;
    mDemuxThread = new std::thread(&RHMediaDemux::readThread, this);
    return RH_SUCCEED;
}

void RHMediaDemux::stop() {
    mAbortRequest = true;
    notify();
    SAFE_DELETE_THREAD(mDemuxThread)
}

void RHMediaDemux::libffmpeg_log_callback(void *ptr, int level, const char *fmt, va_list vl) {
    if (level > av_log_get_level())
        return;
    ALOGI(fmt, vl);
}

void RHMediaDemux::readThread() {
    ALOGI("%s path:%s, thread id:%lu \n", __FILE_NAME__,mMediaPath.c_str(), GetThreadId());

//    av_log_set_level(AV_LOG_TRACE);
    av_log_set_level(AV_LOG_DEBUG);
//    av_log_set_callback(&RHMediaDemux::libffmpeg_log_callback);

    int                            ret = R_CODE_NONE;
    int                          isEof = 0;
    int                        isError = 0;
    int                    readComlete = 0;

    AVPacket                      *pkt = NULL;

    do {
        //1.创建封装格式上下文
        mAVFormatContext = avformat_alloc_context();

        //2.打开文件
        int result = avformat_open_input(&mAVFormatContext, mMediaPath.c_str(), NULL, NULL);
        if (result != 0) {
            ALOGE("%s avformat_open_input faild:%s\n", mMediaPath.c_str(), av_err2str(result));
            ret = R_ERROR_MEDIA_DEMUX_READ_FILE;
            goto END;
        }

        av_format_inject_global_side_data(mAVFormatContext);

        //3.获取音视频流信息
        result = avformat_find_stream_info(mAVFormatContext, NULL);
        if (result < 0) {
            ALOGE("decode get media info faild result = %d.errorStr:%s\n", result, av_err2str(result));
            ret = R_ERROR_MEDIA_DEMUX_FIND_INFO;
            goto END;
        }

        //4.获取音视频流索引
        for (int i = 0; i < mAVFormatContext->nb_streams; i++) {
            int codecType = mAVFormatContext->streams[i]->codecpar->codec_type;
            if (codecType == AVMEDIA_TYPE_VIDEO) {
                mVideoStreamIndex = av_find_best_stream(mAVFormatContext,
                                                        AVMEDIA_TYPE_VIDEO,
                                                        i, -1, NULL, 0);
            } else if(codecType == AVMEDIA_TYPE_AUDIO) {
                mAudioStreamIndex = av_find_best_stream(mAVFormatContext,
                                                        AVMEDIA_TYPE_AUDIO,
                                                        i, -1, NULL, 0);
            }
        }

        ALOGI("find video and audio stream:[%d,%d] \n", mVideoStreamIndex, mAudioStreamIndex);
        if(!mEnableVideoStream) {
            ALOGI("enable video stream:%d", mEnableVideoStream);
            mVideoStreamIndex = -1;
        }
        if(!mEnableAudioStream) {
            ALOGI("enable audio stream:%d", mEnableAudioStream);
            mAudioStreamIndex = -1;
        }

        if(mVideoStreamIndex == -1 && mAudioStreamIndex == -1) {
            ALOGE("not found video stream.\n");
            ret = R_ERROR_MEDIA_DEMUX_NOT_FIND_VIDEO_AUDIO;
            goto END;
        }

        if (mAVFormatContext->duration != AV_NOPTS_VALUE) {
            mDuration = mAVFormatContext->duration / AV_TIME_BASE * 1000;
            ALOGI("duration = %d\n", (int) mDuration);
        }

        bool isPrepare = true;
        if(mDelegate) {
            isPrepare = mDelegate->onPrepare(mAVFormatContext, mDuration, mVideoStreamIndex, mAudioStreamIndex);
        }
        if(!isPrepare) {
            ALOGE("onPrepare false \n");
            goto END;
        }

        pkt = av_packet_alloc();

        while (true) {
            AGAIN:
            if(mAbortRequest) {
                goto END;
            }

            bool isPause = mPauseOn;
            if (mDelegate) {
                isPause = mDelegate->isPause();
            }
            if (isPause) {
                wait();
                goto AGAIN;
            }

            long cacheSize = 0;
            if(mDelegate) {
                cacheSize = mDelegate->getCacheSize();
            }
            if (cacheSize > mMaxCacheSize) {
                wait();
                goto AGAIN;
            }

            if (isEof && !mPauseOn) {
                bool isDecodeEof = true;
                if (mDelegate ) {
                    isDecodeEof = mDelegate->isEof();
                }
                if (isDecodeEof) {
                    if (readComlete) {
                        wait();
                    } else {
                        readComlete = 1;
                        if (isError) {
                            ALOGI("callback faild\n");
                            ret = R_ERROR_MEDIA_DEMUX_READ_FRAME;
                            goto END;
                        } else {
                            if(mDelegate) {
                                ALOGI("demux complete\n");
                                mDelegate->onDemuxComplete();
                            }
                        }
                    }
                }
            }

            //读取压缩的原始数据
            if(mDemuxLock != nullptr) {
                mDemuxLock->lock();
            }
            int readFrame = av_read_frame(mAVFormatContext, pkt);
            if(mDemuxLock != nullptr) {
                mDemuxLock->unlock();
            }
            if (readFrame < 0) {
                int pb_eof = 0, pb_error = 0;
                if ((readFrame == AVERROR_EOF || avio_feof(mAVFormatContext->pb)) && !isEof) {
                    pb_eof = 1;
                }
                if (mAVFormatContext->pb && mAVFormatContext->pb->error) {
                    pb_eof = 1;
                    pb_error = 1;
                }
                if (readFrame == AVERROR_EXIT) {
                    pb_eof = 1;
                    pb_error = AVERROR_EXIT;
                }

                if (pb_eof || pb_error) {
                    if(mDelegate) {
                        mDelegate->onFlush();
                    }
                    isEof = 1;
                    isError = pb_error;
                    av_packet_unref(pkt);
                    wait();
                    goto AGAIN;
                }
            } else {
                isEof = 0;
                isError = 0;
                readComlete = 0;
            }
            if (isEof) {
                av_packet_unref(pkt);
                wait();
                goto AGAIN;
            }

            if(mDelegate) {
                if (mEnableVideoStream && pkt->stream_index == mVideoStreamIndex) {
                    mDelegate->onPacket(AVMEDIA_TYPE_VIDEO, pkt);
                } else if (mEnableAudioStream && pkt->stream_index == mAudioStreamIndex) {
                    mDelegate->onPacket(AVMEDIA_TYPE_AUDIO, pkt);
                }
            }
            av_packet_unref(pkt);
        }
    } while (false);

    END:
    if(pkt) {
        SAFE_FREE_PACKET(pkt)
    }
    if(mDelegate) {
        if(ret != R_CODE_NONE) {
            mDelegate->onError(ret);
        }
        mDelegate->onDemuxDestroy();
    }

    if (mAVFormatContext) {
        avformat_close_input(&mAVFormatContext);
        avformat_free_context(mAVFormatContext);
    }
}
