#ifndef VIDEOSAVER_H
#define VIDEOSAVER_H
#include "QTimer"
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QString>
#include <QDebug>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
}
#include "framebuffer.h"

class VideoSaver : public QThread {
    Q_OBJECT
public:
    VideoSaver(FrameBuffer &frameBuffer, AVCodecContext *decoder_ctx,AVFormatContext *decode_fmt_ctx,const QString &filename);
    ~VideoSaver();
    void run() override;
    void stop();
    Qimage getFrame(const QImage &image);

private:

    FrameBuffer &frameBuffer;
    AVFormatContext *encode_fmt_ctx;
    AVCodecContext *encode_ctx;
    AVFormatContext *decode_fmt_ctx;
    AVCodecContext *decoder_ctx;
    AVStream *video_stream;
    AVCodec *encodec;
    AVFrame *enframe;
    AVPacket *enpkt;
    struct SwsContext *sws_ctx;
    QString filename;
    bool stopFlag;
};

#endif // VIDEOSAVER_H
