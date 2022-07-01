//
// Created by Hope liao on 2022/6/16.
//

#include "RHPNGEncoder.h"
#include "RHAppDef.h"
#include "RHCommonDefine.h"

extern "C" {
#include <libavutil/avassert.h>
#include <libavutil/channel_layout.h>
#include <libavutil/opt.h>
#include <libavutil/mathematics.h>
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
}

RHPNGEncoder::RHPNGEncoder() {
}

RHPNGEncoder::~RHPNGEncoder() {

}

int RHPNGEncoder::encodeImage(const char *_path, AVFrame *_pInFrame) {
    int                             ret = RH_SUCCEED;
    AVCodecContext      * pCodecContext = NULL;
    AVFormatContext    * pFormatContext = NULL;
    AVCodec                  * pAVCodec = NULL;
    AVStream                * pAVStream = NULL;

    AVPacket pkt = {};
    pkt.data = nullptr;
    pkt.size = 0;
    av_init_packet(&pkt);
    do {
        avformat_alloc_output_context2(&pFormatContext, NULL, NULL, _path);
        if (!pFormatContext) {
            ALOGE("_encode_record Could not deduce output format from file");
            ret = RH_ERROR_IMAGE_ENCODE;
            goto END;
        }

        pFormatContext->oformat = av_guess_format(NULL, NULL, "image/png");

        if (!(pFormatContext->oformat->flags & AVFMT_NOFILE)) {
            ret = avio_open(&pFormatContext->pb, _path, AVIO_FLAG_READ_WRITE);
            if (ret < 0) {
                ALOGE("conld not open file:%s, err:%s", _path, av_err2str(ret));
                ret = RH_ERROR_IMAGE_ENCODE;
                goto END;
            }
        }

        pAVStream = avformat_new_stream(pFormatContext, NULL);

        pCodecContext = avcodec_alloc_context3(pAVCodec);
        if (!pCodecContext) {
            ALOGE("create video stream Could not alloc an encoding context");
            ret = RH_ERROR_IMAGE_ENCODE;
            goto END;
        }
        av_opt_set(pCodecContext->priv_data, "tune", "zerolatency", 0); //告诉编码器不用等待缓冲区填满，接收到数据即开始编码

        pCodecContext->codec_id = pFormatContext->oformat->video_codec;
        pCodecContext->codec_type = AVMEDIA_TYPE_VIDEO;
        pCodecContext->pix_fmt = AV_PIX_FMT_RGBA;

        pCodecContext->width = _pInFrame->width;
        pCodecContext->height = _pInFrame->height;

        pCodecContext->time_base = (AVRational) {1, 25};
        //输出格式信息
        av_dump_format(pFormatContext, 0, _path, 1);

        pAVCodec = avcodec_find_encoder(pFormatContext->oformat->video_codec);

        ret = avcodec_open2(pCodecContext, pAVCodec, NULL);
        //7. 打开编码器
        if (ret < 0) {
            ALOGE("open encoder fail!:%s", av_err2str(ret));
            ret = RH_ERROR_IMAGE_ENCODE;
            goto END;
        }

        ret = avcodec_parameters_from_context(pAVStream->codecpar, pCodecContext);
        if (ret < 0) {
            ALOGE("Failed to copy encoder parameters to video stream,error = %s", av_err2str(ret));
            ret = RH_ERROR_IMAGE_ENCODE;
            goto END;
        }

        //Write Header
        ret = avformat_write_header(pFormatContext, NULL);
        if(ret < 0) {
            ret = RH_ERROR_IMAGE_ENCODE;
            ALOGE("avformat_write_header faild:%s", av_err2str(ret));
            goto END;
        }

        ret = avcodec_send_frame(pCodecContext, _pInFrame);
        if (ret == AVERROR_EOF) {
            ret = RH_ERROR_IMAGE_ENCODE;
            goto END;
        } else if (ret < 0) {
            ret = RH_ERROR_IMAGE_ENCODE;
            ALOGE("avcodec_send_frame faild:%s", av_err2str(ret));
            goto END;
        }
        avcodec_send_frame(pCodecContext, NULL);

        while (ret >= 0) {
            //接收编码好的数据
            ret = avcodec_receive_packet(pCodecContext, &pkt);  //从编码器内部缓存中提取编码的数据包
            if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) { //AVERROR(EAGAIN): 表示当前编码没有任何问题，但是输入的音视频帧不够，所以当前没有packet输出，需要继续送入frame进行编码码
                ret = RH_SUCCEED;
                goto END;
            }
            if (ret < 0) {
                ret = RH_ERROR_IMAGE_ENCODE;
                goto END;
            }
            pkt.stream_index = pAVStream->index;
            //将pkt的所有时间基都转换为dst的时间基
            av_packet_rescale_ts(&pkt, pCodecContext->time_base, pAVStream->time_base);
            //写入文件
            ret = av_interleaved_write_frame(pFormatContext, &pkt);
            av_packet_unref(&pkt);
        }

        if (pFormatContext) {
            av_write_trailer(pFormatContext);
        }
    } while (false);

    END:
    if (pCodecContext) {
        avcodec_close(pCodecContext);
        avcodec_free_context(&pCodecContext);
    }
    if(pFormatContext) {
        av_write_trailer(pFormatContext);
        if (!(pFormatContext->oformat->flags & AVFMT_NOFILE)) {
            if(pFormatContext->pb) {
                avio_flush(pFormatContext->pb);
                avio_closep(&pFormatContext->pb);
            }
        }
        avformat_free_context(pFormatContext);
    }
    av_packet_unref(&pkt);
    return ret;
}