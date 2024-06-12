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
    VideoSaver(FrameBuffer &frameBuffer, AVCodecContext *decoder_ctx, const QString &filename);
    void run() override;
    void stop();

private:
    FrameBuffer &frameBuffer;
    AVCodecContext *decoder_ctx;
    QString filename;
    bool stopFlag;
};

#endif // VIDEOSAVER_H
