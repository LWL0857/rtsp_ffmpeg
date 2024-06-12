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
    void run() override;
    void stop();
    AVCodecContext* getCodecContext() const;

private:
    QString url;
    FrameBuffer &frameBuffer;
    AVCodecContext *codec_ctx;
    bool stopFlag;
};

#endif // VIDEODECODER_H
