#ifndef VIDEODECODER_H
#define VIDEODECODER_H

#include <QThread>
#include <QString>
#include <QDebug>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}
#include "framebuffer.h"

class VideoDecoder : public QThread
{
public:
    VideoDecoder(const QString &url, FrameBuffer &frameBuffer);
    ~VideoDecoder();
    void run() override;
    void stop();
    AVCodecContext* getCodecContext() const;
    AVFormatContext* getFormatContext() const;

private:
    QString url;
    FrameBuffer &frameBuffer;
    AVCodecContext *decodec_ctx;
    AVFormatContext *decode_fmt_ctx;
    AVCodecParameters *decodecpar;
    AVCodec *decodec;
    AVFrame *deframe;
    AVPacket *depkt;
    bool stopFlag;
};

#endif // VIDEODECODER_H
