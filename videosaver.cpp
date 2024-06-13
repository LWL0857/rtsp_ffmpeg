#include "videosaver.h"

VideoSaver::VideoSaver(FrameBuffer &frameBuffer, AVCodecContext *decoder_ctx, AVFormatContext *decode_fmt_ctx,const QString &filename)
    : frameBuffer(frameBuffer), decoder_ctx(decoder_ctx),decode_fmt_ctx(decode_fmt_ctx),filename(filename), stopFlag(false) {
    encode_fmt_ctx = nullptr;
    encode_ctx = nullptr;
    video_stream = nullptr;
    encodec = nullptr;
    enframe = nullptr;
    enpkt = nullptr;
    sws_ctx = nullptr;

}




void VideoSaver::run() {

    bool codec_initialized = false;
    enframe = av_frame_alloc();
    enpkt=av_packet_alloc();
    int64_t pts = 0;

    if (avformat_alloc_output_context2(&encode_fmt_ctx, nullptr, nullptr, filename.toStdString().c_str()) < 0) {
        qDebug() << "Failed to create output context";
        return;
    }

    encodec = avcodec_find_encoder(AV_CODEC_ID_H264);
    if (!encodec) {
        qDebug() << "Failed to find encoder";
        return;
    }

    video_stream = avformat_new_stream(encode_fmt_ctx, encodec);
    if (!video_stream) {
        qDebug() << "Failed to create new stream";
        return;
    }
    encode_ctx = avcodec_alloc_context3(encodec);
    if (!encode_ctx) {
        qDebug() << "Failed to allocate codec context";
        return;
    }
    AVFrame* decodedFrame1 = frameBuffer.peek();
        // 初始化编码器上下文


        encode_ctx->codec_id = encodec->id;
        encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
        encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
        encode_ctx->width = decodedFrame1->width;
        encode_ctx->height = decodedFrame1->height;
        encode_ctx->time_base = AVRational{1, 25};
        encode_ctx->framerate = AVRational{25, 1};
        encode_ctx->sample_aspect_ratio = decodedFrame1->sample_aspect_ratio;
        encode_ctx->color_range = decodedFrame1->color_range;
        encode_ctx->color_primaries = decodedFrame1->color_primaries;
        encode_ctx->color_trc = decodedFrame1->color_trc;
        encode_ctx->colorspace = decodedFrame1->colorspace;
        encode_ctx->chroma_sample_location = decodedFrame1->chroma_location;

        if (encode_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
            encode_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
        }

        if (avcodec_open2(encode_ctx, encodec, nullptr) < 0) {
            qDebug() << "Failed to open codec";
            return;
        }

        if (avcodec_parameters_from_context(video_stream->codecpar, encode_ctx) < 0) {
            qDebug() << "Failed to copy codec parameters";
            return;
        }

        if (!(encode_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
            if (avio_open(&encode_fmt_ctx->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
                qDebug() << "Failed to open output file";
                return;
            }
        }
        if (avformat_write_header(encode_fmt_ctx, nullptr) < 0) {
            qDebug() << "Failed to write header";
            return;
        }
        if (av_frame_get_buffer(enframe, 32) < 0) {
            qDebug() << "Failed to allocate frame buffer";
            return;
        }








    while (!stopFlag) {
        AVFrame* decodedFrame = frameBuffer.peek();
//        if (!codec_initialized) {
//            // 初始化编码器上下文
//            encode_ctx = avcodec_alloc_context3(encodec);
//            if (!encode_ctx) {
//                qDebug() << "Failed to allocate codec context";
//                return;
//            }
            
//            encode_ctx->codec_id = encodec->id;
//            encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
//            encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//            encode_ctx->width = decodedFrame->width;
//            encode_ctx->height = decodedFrame->height;
//            encode_ctx->time_base = AVRational{1, 25};
//            encode_ctx->framerate = AVRational{25, 1};
//            encode_ctx->sample_aspect_ratio = decodedFrame->sample_aspect_ratio;
//            encode_ctx->color_range = decodedFrame->color_range;
//            encode_ctx->color_primaries = decodedFrame->color_primaries;
//            encode_ctx->color_trc = decodedFrame->color_trc;
//            encode_ctx->colorspace = decodedFrame->colorspace;
//            encode_ctx->chroma_sample_location = decodedFrame->chroma_location;
//            if (avcodec_parameters_from_context(video_stream->codecpar, encode_ctx) < 0) {
//                qDebug() << "Failed to copy codec parameters";
//                return;
//            }

            
//            if (avcodec_open2(encode_ctx, encodec, nullptr) < 0) {
//                qDebug() << "Failed to open codec";
//                return;
//            }
//            if (decode_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
//                 encode_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//            }
            
//            if (!(encode_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
//                if (avio_open(&encode_fmt_ctx->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
//                    qDebug() << "Failed to open output file";
//                    return;
//                }
//            }
//            if (avformat_write_header(encode_fmt_ctx, nullptr) < 0) {
//                qDebug() << "Failed to write header";
//                return;
//            }
            
//            codec_initialized = true;
//        }
        sws_ctx = sws_getContext(decodedFrame->width, decodedFrame->height, AV_PIX_FMT_YUV420P,
                                 decodedFrame->width, decodedFrame->height, AV_PIX_FMT_YUV420P,
                                 SWS_BILINEAR, nullptr, nullptr, nullptr);
//        AV_PIX_FMT_RGB24

        sws_scale(sws_ctx, decodedFrame->data, decodedFrame->linesize, 0, encode_ctx->height, enframe->data, enframe->linesize);
        enframe->pts = pts++;

        if (avcodec_send_frame(encode_ctx, enframe) >= 0) {
            while (avcodec_receive_packet(encode_ctx, enpkt) >= 0) {
                av_packet_rescale_ts(enpkt, encode_ctx->time_base, video_stream->time_base);
                enpkt->stream_index = video_stream->index;
                av_interleaved_write_frame(decode_fmt_ctx, enpkt);
                av_packet_unref(enpkt);
            }
        }

        av_frame_free(&decodedFrame);
    }

    av_write_trailer(decode_fmt_ctx);

    av_frame_free(&enframe);
    av_packet_free(&enpkt);
    avcodec_free_context(&encode_ctx);
    avformat_free_context(decode_fmt_ctx);
    // Deinitialize network
    avformat_network_deinit();
}

void VideoSaver::stop() {
    stopFlag = true;
}
