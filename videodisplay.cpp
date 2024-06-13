#include "videodisplay.h"

VideoDisplay::VideoDisplay(QLabel *videoLabel, FrameBuffer &frameBuffer)
    : videoLabel(videoLabel), frameBuffer(frameBuffer), stopFlag(false) {

    encode_fmt_ctx = nullptr;
    encode_ctx = nullptr;
    video_stream = nullptr;
    encodec = nullptr;
    enframe = nullptr;
    enpkt = nullptr;
    sws_ctx_en = nullptr;



    }


void initSaver(AVCodecContext *decoder_ctx, AVFormatContext *decode_fmt_ctx, const QString &filename)
{
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
    enframe = av_frame_alloc();
    if (!enframe) {
        qDebug() << "Failed to allocate frame";
        return;
    }

    enpkt = av_packet_alloc();
    if (!enpkt) {
        qDebug() << "Failed to allocate packet";
        return;
    }
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
    if (!decodedFrame1) {
        qDebug() << "Failed to peek frame from buffer";
        return;
    }

    encode_ctx->codec_id = encodec->id;
    encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
    encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
    encode_ctx->width = decodedFrame1->width;
    encode_ctx->height = decodedFrame1->height;
    encode_ctx->time_base = decode_fmt_ctx->streams[video_stream_index]->time_base;
    encode_ctx->framerate = decoder_ctx->framerate;
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

    enframe->format = encode_ctx->pix_fmt;
    enframe->width = encode_ctx->width;
    enframe->height = encode_ctx->height;

    if (av_frame_get_buffer(enframe, 32) < 0) {
        char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
        av_strerror(AVERROR(errno), errbuf, AV_ERROR_MAX_STRING_SIZE);
        qDebug() << "Failed to allocate frame buffer:" << errbuf;
        return;
    }

    sws_ctx = sws_getContext(decodedFrame1->width, decodedFrame1->height, AV_PIX_FMT_YUV420P,
                             decodedFrame1->width, decodedFrame1->height, AV_PIX_FMT_YUV420P,
                             SWS_BILINEAR, nullptr, nullptr, nullptr);


}


void VideoDisplay::run() {
    AVFrame *frame = nullptr;
    SwsContext *sws_ctx = nullptr;

    while (!stopFlag) {
        frame = frameBuffer.pop();

        if (!frame) continue;
        if(saveFlag)
        {

            

        }

        if (!sws_ctx) {
            sws_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format,
                                     frame->width, frame->height, AV_PIX_FMT_RGB24,
                                     SWS_BILINEAR, nullptr, nullptr, nullptr);
        }

        QImage img(frame->width, frame->height, QImage::Format_RGB888);
        uint8_t *data[1] = { img.bits() };
        int linesize[1] = { img.bytesPerLine() };

        sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, data, linesize);

        emit frameReady(img);
        av_frame_free(&frame);
    }

    if (sws_ctx) {
        sws_freeContext(sws_ctx);
    }
}

void VideoDisplay::stop() {
    stopFlag = true;
}
