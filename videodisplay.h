#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H

#include <QThread>


#include <QLabel>
#include <QImage>
#include <QPixmap>
extern "C" {
#include <libavutil/frame.h>
#include <libswscale/swscale.h>
}
#include "framebuffer.h"

class VideoDisplay : public QThread {
    Q_OBJECT
public:
    VideoDisplay(QLabel *videoLabel, FrameBuffer &frameBuffer);
    void run() override;
    void stop();

signals:
    void frameReady(const QImage &image);

private:
    QLabel *videoLabel;
    FrameBuffer &frameBuffer;
    bool stopFlag;
    bool saveFlag;
    AVFormatContext *encode_fmt_ctx;
    AVCodecContext *encode_ctx;
    AVFormatContext *decode_fmt_ctx;
    AVCodecContext *decoder_ctx;
    AVStream *video_stream;
    AVCodec *encodec;
    AVFrame *enframe;
    AVPacket *enpkt;
    struct SwsContext *sws_ctx_en;
    QString filename;
    bool stopFlag;
};

#endif // VIDEODISPLAY_H
