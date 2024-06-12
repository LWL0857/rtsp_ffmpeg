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

    video_stream->time_base = decode_fmt_ctx->streams[0]->time_base;

//    encode_ctx = avcodec_alloc_context3(encodec);
//    if (!encode_ctx) {
//        qDebug() << "Failed to allocate codec context";
//        return;
//    }

//    encode_ctx->codec_id = encodec->id;
//    encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
//    encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//    encode_ctx->width = decoder_ctx->width;
//    encode_ctx->height = decoder_ctx->height;
//    encode_ctx->time_base = AVRational{1, 25};
//    encode_ctx->framerate = AVRational{25, 1};
//    encode_ctx->time_base = fmt_ctx->streams[0]->time_base;
//    encode_ctx->width = fmt_ctx->streams[0]->codecpar->width;
//    encode_ctx->height = fmt_ctx->streams[0]->codecpar->height;
//    encode_ctx->sample_aspect_ratio = st->sample_aspect_ratio = frame->sample_aspect_ratio;
//    encode_ctx->pix_fmt = enframe->format;
//    encode_ctx->color_range            = enframe->color_range;
//    encode_ctx->color_primaries        = enframe->color_primaries;
//    encode_ctx->color_trc              = enframe->color_trc;
//    encode_ctx->colorspace             = enframe->colorspace;
//    encode_ctx->chroma_sample_location = enframe->chroma_location;







    if (encode_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
        encode_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

//    if (avcodec_open2(encode_ctx, encodec, nullptr) < 0) {
//        qDebug() << "Failed to open codec";
//        return;
//    }

//    if (avcodec_parameters_from_context(video_stream->codecpar, codec_ctx) < 0) {
//        qDebug() << "Failed to copy codec parameters";
//        return;
//    }

//    if (!(fmt_ctx_out->oformat->flags & AVFMT_NOFILE)) {
//        if (avio_open(&fmt_ctx_out->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
//            qDebug() << "Failed to open output file";
//            return;
//        }
//    }

    if (avformat_write_header(encode_fmt_ctx, nullptr) < 0) {
        qDebug() << "Failed to write header";
        return;
    }

    enframe = av_frame_alloc();
    enpkt=av_packet_alloc();
    frame->format = codec_ctx->pix_fmt;
    frame->width = codec_ctx->width;
    frame->height = codec_ctx->height;

    if (av_frame_get_buffer(frame, 32) < 0) {
        qDebug() << "Failed to allocate frame buffer";
        return;
    }

    sws_ctx = sws_getContext(codec_ctx->width, codec_ctx->height, AV_PIX_FMT_YUV420P,
                             codec_ctx->width, codec_ctx->height,  AV_PIX_FMT_YUV420P,
                             SWS_BILINEAR, nullptr, nullptr, nullptr);
//    sws_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format,
//                             frame->width, frame->height, AV_PIX_FMT_RGB24,
//                             SWS_BILINEAR, nullptr, nullptr, nullptr);


    int64_t pts = 0;
    while (!stopFlag) {
        AVFrame* decodedFrame = frameBuffer.peek();
        sws_ctx = sws_getContext(decodedFrame->width, decodedFrame->height, AV_PIX_FMT_YUV420P,
                                 decodedFrame->width, decodedFrame->height, AV_PIX_FMT_RGB24,
                                 SWS_BILINEAR, nullptr, nullptr, nullptr);

        sws_scale(sws_ctx, decodedFrame->data, decodedFrame->linesize, 0, codec_ctx->height, frame->data, frame->linesize);
        frame->pts = pts++;

        if (avcodec_send_frame(codec_ctx, frame) >= 0) {
            while (avcodec_receive_packet(codec_ctx, pkt) >= 0) {
                av_packet_rescale_ts(pkt, codec_ctx->time_base, video_stream->time_base);
                pkt->stream_index = video_stream->index;
                av_interleaved_write_frame(fmt_ctx_out, pkt);
                av_packet_unref(pkt);
            }
        }

        av_frame_free(&decodedFrame);
    }

    av_write_trailer(fmt_ctx_out);

    av_frame_free(&frame);
    av_packet_free(&pkt);
    avcodec_free_context(&codec_ctx);
    avformat_free_context(fmt_ctx_out);
}

void VideoSaver::stop() {
    stopFlag = true;
}
