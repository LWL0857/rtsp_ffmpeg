#include "videodecoder.h"

VideoDecoder::VideoDecoder(const QString &url, FrameBuffer &frameBuffer)
    : url(url), frameBuffer(frameBuffer), codec_ctx(nullptr), stopFlag(false) {}
//VideoDecoder::VideoDecoder(QObject *parent)
//    : QThread{parent}
//{

//}

//void VideoDecoder::run() {
//    AVFormatContext *fmt_ctx = nullptr;
//    AVCodec *codec = nullptr;
//    AVCodecParameters *codecpar = nullptr;
//    AVFrame *frame = nullptr;
//    AVPacket *pkt = nullptr;

//    qDebug() << url;
//    if (avformat_open_input(&fmt_ctx, url.toStdString().c_str(), nullptr, nullptr) < 0) {
//        qDebug() << "Failed to open input";
//        return;
//    }
//    qDebug() << url;

//    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
//        qDebug() << "Failed to find stream info";
//        return;
//    }

//    int video_stream_index = -1;
//    for (unsigned int i = 0; i < fmt_ctx->nb_streams; ++i) {
//        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            video_stream_index = i;
//            break;
//        }
//    }

//    if (video_stream_index == -1) {
//        qDebug() << "Failed to find video stream";
//        return;
//    }

//    codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
//    codec = avcodec_find_decoder(codecpar->codec_id);
//    if (!codec) {
//        qDebug() << "Failed to find decoder";
//        return;
//    }

//    codec_ctx = avcodec_alloc_context3(codec);
//    if (!codec_ctx) {
//        qDebug() << "Failed to allocate codec context";
//        return;
//    }

//    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
//        qDebug() << "Failed to copy codec parameters to context";
//        return;
//    }

//    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
//        qDebug() << "Failed to open codec";
//        return;
//    }

//    // 添加保存为XVID格式的代码
//    codec_ctx->codec_id = AV_CODEC_ID_XVID;
//    codec_ctx->bit_rate = 400000; // 设置比特率
//    codec_ctx->width = codecpar->width;
//    codec_ctx->height = codecpar->height;
//    codec_ctx->time_base = {1, codecpar->sample_rate};
//    codec_ctx->gop_size = 10;
//    codec_ctx->max_b_frames = 1;
//    codec_ctx->pix_fmt = AV_PIX_FMT_YUV420P;

//    frame = av_frame_alloc();
//    pkt = av_packet_alloc();

//    while (!stopFlag) {
//        if (av_read_frame(fmt_ctx, pkt) >= 0) {
//            if (pkt->stream_index == video_stream_index) {
//                if (avcodec_send_packet(codec_ctx, pkt) >= 0) {
//                    while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
//                        frameBuffer.push(frame);
//                    }
//                }
//            }
//            av_packet_unref(pkt);
//        }
//    }

//    av_frame_free(&frame);
//    av_packet_free(&pkt);
//    avcodec_free_context(&codec_ctx);
//    avformat_close_input(&fmt_ctx);
//}

void VideoDecoder::run() {
    AVFormatContext *fmt_ctx = nullptr;
    AVCodec *codec = nullptr;
    AVCodecParameters *codecpar = nullptr;
    AVFrame *frame = nullptr;
    AVPacket *pkt = nullptr;



    qDebug()<<url;
    if (avformat_open_input(&fmt_ctx, url.toStdString().c_str(), nullptr, nullptr) < 0) {
        qDebug() << "Failed to open input";
        return;
    }
    qDebug()<<url;

    if (avformat_find_stream_info(fmt_ctx, nullptr) < 0) {
        qDebug() << "Failed to find stream info";
        return;
    }

    int video_stream_index = -1;
    for (unsigned int i = 0; i < fmt_ctx->nb_streams; ++i) {
        if (fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
    }

    if (video_stream_index == -1) {
        qDebug() << "Failed to find video stream";
        return;
    }

    codecpar = fmt_ctx->streams[video_stream_index]->codecpar;
    codec = avcodec_find_decoder(codecpar->codec_id);
    if (!codec) {
        qDebug() << "Failed to find decoder";
        return;
    }

    codec_ctx = avcodec_alloc_context3(codec);
    if (!codec_ctx) {
        qDebug() << "Failed to allocate codec context";
        return;
    }

    if (avcodec_parameters_to_context(codec_ctx, codecpar) < 0) {
        qDebug() << "Failed to copy codec parameters to context";
        return;
    }

    if (avcodec_open2(codec_ctx, codec, nullptr) < 0) {
        qDebug() << "Failed to open codec";
        return;
    }

    frame = av_frame_alloc();
    pkt = av_packet_alloc();

    while (!stopFlag) {
        if (av_read_frame(fmt_ctx, pkt) >= 0) {
            if (pkt->stream_index == video_stream_index) {
                if (avcodec_send_packet(codec_ctx, pkt) >= 0) {
                    while (avcodec_receive_frame(codec_ctx, frame) >= 0) {
                        frameBuffer.push(frame);
                    }
                }
            }
            av_packet_unref(pkt);
        }
    }

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_close_input(&fmt_ctx);
}

void VideoDecoder::stop() {
    stopFlag = true;
}

AVCodecContext* VideoDecoder::getCodecContext() const {
    return codec_ctx;
}
