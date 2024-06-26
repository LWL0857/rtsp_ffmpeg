// #include "videosaver.h"

// VideoSaver::VideoSaver(FrameBuffer &frameBuffer, AVCodecContext *decoder_ctx, AVFormatContext *decode_fmt_ctx,const QString &filename)
//     : frameBuffer(frameBuffer), decoder_ctx(decoder_ctx),decode_fmt_ctx(decode_fmt_ctx),filename(filename), stopFlag(false) {
//     encode_fmt_ctx = nullptr;
//     encode_ctx = nullptr;
//     video_stream = nullptr;
//     encodec = nullptr;
//     enframe = nullptr;
//     enpkt = nullptr;
//     sws_ctx = nullptr;

// }

// VideoSaver::~VideoSaver()
// {
//     avcodec_free_context(&encode_ctx);
//     avformat_free_context(encode_fmt_ctx);
//     av_frame_free(&enframe);
//     av_packet_free(&enpkt);
// }




// void VideoSaver::run() {

//     int video_stream_index = -1;
//     for (unsigned int i = 0; i < decode_fmt_ctx->nb_streams; ++i) {
//         if (decode_fmt_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//             video_stream_index = i;
//             break;
//         }
//     }

//     if (video_stream_index == -1) {
//         qDebug() << "Failed to find video stream";
//         return;
//     }

//     bool codec_initialized = false;
//     enframe = av_frame_alloc();
//     if (!enframe) {
//         qDebug() << "Failed to allocate frame";
//         return;
//     }
//     enpkt=av_packet_alloc();
//     if (!enpkt) {
//         qDebug() << "Failed to allocate packet";
//         return;
//     }
//     int64_t pts = 0;

//     if (avformat_alloc_output_context2(&encode_fmt_ctx, nullptr, nullptr, filename.toStdString().c_str()) < 0) {
//         qDebug() << "Failed to create output context";
//         return;
//     }

//     encodec = avcodec_find_encoder(AV_CODEC_ID_H264);
//     if (!encodec) {
//         qDebug() << "Failed to find encoder";
//         return;
//     }

//     video_stream = avformat_new_stream(encode_fmt_ctx, encodec);
//     if (!video_stream) {
//         qDebug() << "Failed to create new stream";
//         return;
//     }
//     // video_stream->time_base = decode_fmt_ctx->streams[video_stream_index]->time_base;


//     encode_ctx = avcodec_alloc_context3(encodec);
//     if (!encode_ctx) {
//         qDebug() << "Failed to allocate codec context";
//         return;
//     }
//     AVFrame* decodedFrame1 = frameBuffer.peek();
//      if (!decodedFrame1) {
//         qDebug() << "Failed to peek frame from buffer";
//         return;
//     }
//     // 初始化编码器上下文


//     encode_ctx->codec_id = encodec->id;
//     encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
//     encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
//     encode_ctx->width = decodedFrame1->width;
//     encode_ctx->height = decodedFrame1->height;
//     encode_ctx->time_base = decode_fmt_ctx->streams[video_stream_index]->time_base;

// //        AVRational{1, 25};
//     encode_ctx->framerate =decoder_ctx->framerate;
// //        AVRational{25, 1};
//     encode_ctx->sample_aspect_ratio = decodedFrame1->sample_aspect_ratio;
//     encode_ctx->color_range = decodedFrame1->color_range;
//     encode_ctx->color_primaries = decodedFrame1->color_primaries;
//     encode_ctx->color_trc = decodedFrame1->color_trc;
//     encode_ctx->colorspace = decodedFrame1->colorspace;
//     encode_ctx->chroma_sample_location = decodedFrame1->chroma_location;
//     // encode_ctx->gop_size = 30;
//     // encode_ctx->max_b_frames = 10;
//     // encode_ctx->profile = FF_PROFILE_H264_MAIN;

//     if (encode_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
//         encode_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
//     }

//     if (avcodec_open2(encode_ctx, encodec, nullptr) < 0) {
//         qDebug() << "Failed to open codec";
//         return;
//     }

//     if (avcodec_parameters_from_context(video_stream->codecpar, encode_ctx) < 0) {
//         qDebug() << "Failed to copy codec parameters";
//         return;
//     }

//     if (!(encode_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
//         if (avio_open(&encode_fmt_ctx->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
//             qDebug() << "Failed to open output file";
//             return;
//         }
//     }
//     if (avformat_write_header(encode_fmt_ctx, nullptr) < 0) {
//         qDebug() << "Failed to write header";
//         return;
//     }

//     enframe->format = encode_ctx->pix_fmt;
//     enframe->width = encode_ctx->width;
//     enframe->height = encode_ctx->height;

//    if (av_frame_get_buffer(enframe,32) < 0) {
//         char errbuf[AV_ERROR_MAX_STRING_SIZE] = {0};
//         av_strerror(AVERROR(errno), errbuf, AV_ERROR_MAX_STRING_SIZE);
//         qDebug() << "Failed to allocate frame buffer:" << errbuf;
//         return;
//    }

//     sws_ctx = sws_getContext(decodedFrame1->width, decodedFrame1->height, AV_PIX_FMT_YUV420P,
//                                 decodedFrame1->width, decodedFrame1->height, AV_PIX_FMT_YUV420P,
//                                 SWS_BILINEAR, nullptr, nullptr, nullptr);
// //        AV_PIX_FMT_RGB24
//      qDebug() << "before while";
//     while (!stopFlag) {
//         AVFrame* decodedFrame = frameBuffer.peek();
// //        if (!codec_initialized) {
// //            // 初始化编码器上下文
// //            encode_ctx = avcodec_alloc_context3(encodec);
// //            if (!encode_ctx) {
// //                qDebug() << "Failed to allocate codec context";
// //                return;
// //            }
            
// //            encode_ctx->codec_id = encodec->id;
// //            encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;
// //            encode_ctx->pix_fmt = AV_PIX_FMT_YUV420P;
// //            encode_ctx->width = decodedFrame->width;
// //            encode_ctx->height = decodedFrame->height;
// //            encode_ctx->time_base = AVRational{1, 25};
// //            encode_ctx->framerate = AVRational{25, 1};
// //            encode_ctx->sample_aspect_ratio = decodedFrame->sample_aspect_ratio;
// //            encode_ctx->color_range = decodedFrame->color_range;
// //            encode_ctx->color_primaries = decodedFrame->color_primaries;
// //            encode_ctx->color_trc = decodedFrame->color_trc;
// //            encode_ctx->colorspace = decodedFrame->colorspace;
// //            encode_ctx->chroma_sample_location = decodedFrame->chroma_location;
// //            if (avcodec_parameters_from_context(video_stream->codecpar, encode_ctx) < 0) {
// //                qDebug() << "Failed to copy codec parameters";
// //                return;
// //            }

            
// //            if (avcodec_open2(encode_ctx, encodec, nullptr) < 0) {
// //                qDebug() << "Failed to open codec";
// //                return;
// //            }
// //            if (decode_fmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
// //                 encode_ctx->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
// //            }
            
// //            if (!(encode_fmt_ctx->oformat->flags & AVFMT_NOFILE)) {
// //                if (avio_open(&encode_fmt_ctx->pb, filename.toStdString().c_str(), AVIO_FLAG_WRITE) < 0) {
// //                    qDebug() << "Failed to open output file";
// //                    return;
// //                }
// //            }
// //            if (avformat_write_header(encode_fmt_ctx, nullptr) < 0) {
// //                qDebug() << "Failed to write header";
// //                return;
// //            }
            
// //            codec_initialized = true;
// //        }
//          if (!decodedFrame) {
//             qDebug() << "Failed to peek frame from buffer";
//             continue; // Skip this iteration if no frame is available
//         }
//         sws_scale(sws_ctx, decodedFrame->data, decodedFrame->linesize, 0, encode_ctx->height, enframe->data, enframe->linesize);
//         enframe->pts = pts++;
//             //  qDebug() << "avcodec_send_frame";
//         if (avcodec_send_frame(encode_ctx, enframe) >= 0) {
//             while (avcodec_receive_packet(encode_ctx, enpkt) >= 0) {
//                 //  qDebug() << " av_packet_rescale_ts(enpkt, encode_ctx->time_base, video_stream->time_base);";
//                 //重新调整时间戳（PTS 和 DTS）的函数,将一个 AVPacket 结构体中的时间戳从一种时间基准转换为另一种时间基准
//                 av_packet_rescale_ts(enpkt, encode_ctx->time_base, video_stream->time_base);
//                 //   qDebug() << " enpkt->stream_index = video_stream->index;";
//                 enpkt->stream_index = video_stream->index;
//                      qDebug() << "  av_interleaved_write_frame(decode_fmt_ctx, enpkt);";
//                 av_interleaved_write_frame(encode_fmt_ctx, enpkt);
//                     //  qDebug() << "   av_packet_unref(enpkt);";
//                 av_packet_unref(enpkt);
//             }
//         }

//         av_frame_free(&decodedFrame);
//     }

//     av_write_trailer(encode_fmt_ctx);

   

// }

// void VideoSaver::stop() {
//     stopFlag = true;
// }



#include "videosaver.h"

VideoSaver::VideoSaver(FrameBuffer &frameBuffer, AVCodecContext *decoder_ctx, AVFormatContext *decode_fmt_ctx, const QString &filename)
    : frameBuffer(frameBuffer), decoder_ctx(decoder_ctx), decode_fmt_ctx(decode_fmt_ctx), filename(filename), stopFlag(false) {
    encode_fmt_ctx = nullptr;
    encode_ctx = nullptr;
    video_stream = nullptr;
    encodec = nullptr;
    enframe = nullptr;
    enpkt = nullptr;
    sws_ctx = nullptr;

}

VideoSaver::~VideoSaver() {
    avcodec_free_context(&encode_ctx);
    avformat_free_context(encode_fmt_ctx);
    av_frame_free(&enframe);
    av_packet_free(&enpkt);
}

void VideoSaver::run() {
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

    qDebug() << "before while";
    int64_t pts = 0;
    AVFrame* lastFrame = nullptr;  // 用于保存上一次处理的帧
    while (!stopFlag) {
        AVFrame* decodedFrame = frameBuffer.peek();
        if (!decodedFrame) {
            qDebug() << "Failed to peek frame from buffer";
            continue; // Skip this iteration if no frame is available
        }
        // 如果当前帧与上一次处理的帧相同，则跳过这一轮循环
        if (decodedFrame == lastFrame) {
            continue;
        }

        sws_scale(sws_ctx, decodedFrame->data, decodedFrame->linesize, 0, encode_ctx->height, enframe->data, enframe->linesize);
        enframe->pts = pts++;
        if (avcodec_send_frame(encode_ctx, enframe) >= 0) {
            while (avcodec_receive_packet(encode_ctx, enpkt) >= 0) {
                //转换 AVPacket 的时间基为 输出流的时间基。
               // enpkt->pts = av_rescale_q_rnd(enpkt->pts, encode_fmt_ctx->streams[0]->time_base, video_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
                //enpkt->dts = av_rescale_q_rnd(enpkt->dts, encode_fmt_ctx->streams[0]->time_base, video_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
               // enpkt->duration = av_rescale_q_rnd(enpkt->duration, encode_fmt_ctx->streams[0]->time_base, video_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
                av_packet_rescale_ts(enpkt, encode_fmt_ctx->streams[0]->time_base, video_stream->time_base);
                enpkt->stream_index = video_stream->index;
                if (av_interleaved_write_frame(encode_fmt_ctx, enpkt) < 0) {
                    qDebug() << "Failed to write frame";
                }
                av_packet_unref(enpkt);
            }
        }
           // 释放之前的帧并更新 lastFrame
        if (lastFrame) {
            av_frame_free(&lastFrame);
        }
        lastFrame = decodedFrame;
        // av_frame_free(&decodedFrame);
    }

    av_write_trailer(encode_fmt_ctx);
    if (lastFrame) {
        av_frame_free(&lastFrame);
    }
}

void VideoSaver::stop() {
    stopFlag = true;
}


