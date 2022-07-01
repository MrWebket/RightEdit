//
// Created by Hope liao on 2022/5/3.
//

#ifndef RIGHTEDIT_RHMEDIADEMUX_H
#define RIGHTEDIT_RHMEDIADEMUX_H

#include "RAppObject.h"
#include <string>
#include <thread>
#include <mutex>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/frame.h>
#include <libavutil/time.h>
#include <libavutil/rational.h>
}

#define R_MAX_PACKET_CACHE_SIZE                (15 * 1024 * 1024)


class RHMediaDemuxDelegate {
public:
    virtual long getCacheSize() = 0;
    virtual bool isEof() = 0;
    virtual bool isPause() = 0;

    virtual bool onPrepare(AVFormatContext *formatContext, long duration, int videoStream, int audioStream) = 0;
    virtual void onFlush() = 0;
    virtual void onPacket(int mediaType, AVPacket * _inPacket) = 0;
    virtual void onDemuxComplete() = 0;
    virtual void onError(int errorCode) = 0;
    virtual void onDemuxDestroy() = 0;
};

class RHLock;
class RHMediaDemux: public RAppObject {

public:
    RHMediaDemux(RHApplication * _app);
    virtual ~RHMediaDemux();

    void loadModel(const char * _path);
    void setDelegate(RHMediaDemuxDelegate * _delegate);
    void setDemuxLock(RHLock * lock);
    void enableStream(bool enableVideo, bool enableAudio);
    int start();
    void stop();
    inline bool isHasVideo() {
        return mEnableVideoStream && mVideoStreamIndex != -1;
    }
    inline bool isHasAudio() {
        return mEnableAudioStream && mAudioStreamIndex != -1;
    }
private:
    static void libffmpeg_log_callback(void *ptr, int level, const char *fmt, va_list vl);

    void reset();
    void readThread();
    void wait();
    void notify();
    RHMediaDemuxDelegate      * mDelegate;
    std::string                 mMediaPath;
    bool                        mAbortRequest;
    bool                        mEnableVideoStream;
    bool                        mEnableAudioStream;

    std::thread               * mDemuxThread;

    AVFormatContext           * mAVFormatContext;

    int                         mVideoStreamIndex;
    int                         mAudioStreamIndex;

    long                        mDuration;
    long                        mMaxCacheSize;
    bool                        mPauseOn;

    std::mutex                  mReadMutex;
    std::condition_variable     mReadCond;

    RHLock                    * mDemuxLock;
};
#endif //RIGHTEDIT_RHMEDIADEMUX_H
