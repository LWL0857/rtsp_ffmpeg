#include "videodecoder.h"

VideoDecoder::VideoDecoder(const QString &url, FrameBuffer &frameBuffer)
    : url(url), frameBuffer(frameBuffer), decodec_ctx(nullptr),decode_fmt_ctx(nullptr),stopFlag(false) {
    decodec = nullptr;
    decodecpar = nullptr;
    deframe = nullptr;
    depkt = nullptr;

}

void VideoDecoder::run() {
    int read_end = 0;
    //网络库init,为了读取网络流
    avformat_network_init();


    decode_fmt_ctx= avformat_alloc_context();
    if (!decode_fmt_ctx) {
        printf("error code %d \n",AVERROR(ENOMEM));
        return ;
    }
    qDebug()<<url;
    if (avformat_open_input(&decode_fmt_ctx, url.toStdString().c_str(), nullptr, nullptr) < 0) {
        qDebug() << "Failed to open input";
        return;
    }
    qDebug()<<url;

    if (avformat_find_stream_info(decode_fmt_ctx, nullptr) < 0) {
        qDebug() << "Failed to find stream info";
        return;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < decode_fmt_ctx->nb_streams; ++i) {
        if (decode_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        qDebug() << "Failed to find video stream";
        return;
    }

    decodecpar = decode_fmt_ctx->streams[video_stream_index]->codecpar;
    decodec = avcodec_find_decoder(decodecpar->codec_id);
    if (!decodec) {
        qDebug() << "Failed to find decoder";
        return;
    }

    decodec_ctx = avcodec_alloc_context3(decodec);
    if (!decodec_ctx) {
        qDebug() << "Failed to allocate codec context";
        return;
    }

    if (avcodec_parameters_to_context(decodec_ctx, decodecpar) < 0) {
        qDebug() << "Failed to copy codec parameters to context";
        return;
    }

    if (avcodec_open2(decodec_ctx, decodec, nullptr) < 0) {
        qDebug() << "Failed to open codec";
        return;
    }

    deframe = av_frame_alloc();
    depkt = av_packet_alloc();

    while (!stopFlag) {


        if (av_read_frame(decode_fmt_ctx, depkt) >= 0) {
            if (depkt->stream_index == video_stream_index) {

                if (avcodec_send_packet(decodec_ctx, depkt) >= 0) {
                    while (avcodec_receive_frame(decodec_ctx, deframe) >= 0) {
                        frameBuffer.push(deframe);
                    }



                }
            }
            av_packet_unref(depkt);
        }



    }

    av_frame_free(&deframe);
    av_packet_free(&depkt);
    avcodec_free_context(&decodec_ctx);
    avformat_close_input(&decode_fmt_ctx);
}

void VideoDecoder::stop() {
    stopFlag = true;
}

AVCodecContext* VideoDecoder::getCodecContext() const {
    return decodec_ctx;
}

AVFormatContext *VideoDecoder::getFormatContext() const
{
     return decode_fmt_ctx;
}
