//
// Created by Hope liao on 2022/5/4.
//

#ifndef RIGHTEDIT_RHTHUMBNAILDECODE2_H
#define RIGHTEDIT_RHTHUMBNAILDECODE2_H


#include "../../common/RHLock.h"
#include "../../common/RHFrameQueue.h"
#include "../../common/RAppObject.h"
#include "../../common/RHMediaDemux.h"
#include "../../common/RHPacketQueue.h"
#include "../../data/RHThumbnailParam.h"

class RHImageConvertThread;
class RHImageEncoderThread;
class RHThumbnailDecode2: public RAppObject, RHMediaDemuxDelegate {
public:
    RHThumbnailDecode2(RHApplication * _app);
    virtual ~RHThumbnailDecode2();

public:
    void pauseOn(bool pause);
    bool start(RHThumbnailParam * _param);
    void stop();
public:
    virtual long getCacheSize() override;
    virtual bool isEof() override;
    virtual bool isPause() override;

    virtual bool onPrepare(AVFormatContext *formatContext, long duration, int videoStream, int audioStream) override;
    virtual void onFlush() override;
    virtual void onPacket(int mediaType, AVPacket * _inPacket) override;
    virtual void onDemuxComplete() override;
    virtual void onError(int errorCode) override;
    virtual void onDemuxDestroy() override;
private:
    void decodeThread();

    void waitDecodeThread();
    void notifyDecodeThread();

    void pushFrameToCache(AVFrame * _pInFrame);

    void convertFrameCallback(AVFrame * _pInFrame);

    RHMediaDemux                   * mDemux;

    AVCodec                       * mCodec;
    AVCodecContext                * mCodecContext;
    AVBufferRef                   * mHwDeviceContext; //硬解的上下文

    int                             mVideoFps;
    int                             mVideoWidth;
    int                             mVideoHeight;
    int                             mGopSize;
    int                             mVideoRotate;
    uint64_t                        mVideoDuration;

    AVRational                      mVideoRate;

    bool                            mAbortRequest;

    bool                            mPauseOn;
    RHThumbnailParam                mThumbnailParam;

    std::thread                   * mDecoderThread;

    RHPacketQueue                   mPackketQueue;

    RHImageConvertThread          * mImageConvertThread;
    RHImageEncoderThread          * mImageEncodeThread;

    std::mutex                      mDecodeMutex;
    std::condition_variable         mDecodeCond;

    long                            mPacketCacheSize;
    uint32_t                        mCacheSize;

    AVStream                      * mVideoStream;
    bool                            mSupportHWDevice;

    AVPixelFormat                   mHWPixelFormat;

    RHLock                          mDecodeLock;

};

#endif //RIGHTEDIT_RHTHUMBNAILDECODE2_H
