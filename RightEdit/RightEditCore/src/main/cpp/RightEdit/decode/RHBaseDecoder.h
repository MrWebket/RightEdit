//
// Created by Hope liao on 2022/7/1.
//

#ifndef RIGHTEDIT_RHBASEDECODER_H
#define RIGHTEDIT_RHBASEDECODER_H

#include "../common/RAppObject.h"
#include <thread>
#include "../common/RHPacketQueue.h"

class AVFormatContext;
class AVCodecContext;
class AVCodec;
class AVStream;
class AVPacket;
class RHBaseDecoder: public RAppObject {
public:
    RHBaseDecoder(RHApplication * _app);
    virtual ~RHBaseDecoder();

public:
    virtual void start();
    virtual void stop();
    virtual int openStream(AVFormatContext *formatContext, int stream);

    virtual int getMediaType() = 0;
    virtual const char * getThreads();

    void pushPacket(AVPacket * pInPacket);
    uint64_t getPacketCacheSize() const;
protected:
    void onDecodeThread();

    AVFormatContext                * mAVFormatContext; //封装格式上下文
    AVCodecContext                 * mAVCodecContext;
    AVCodec                        * mAVCodec; //解码器
    AVStream                       * mAVStream;

    int                              mStreamIndex;

    bool                             mDeocdeWorking;
    std::thread                    * mDecodeThread;

    RHPacketQueue                   mPackketQueue;

    uint64_t                        mPacketCacheSize;
};

#endif //RIGHTEDIT_RHBASEDECODER_H
