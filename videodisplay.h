#ifndef VIDEODISPLAY_H
#define VIDEODISPLAY_H
#include "QTimer"
#include <QDir>
#include <QDateTime>
#include <QThread>
#include <QString>
#include <QDebug>
#include <QLabel>
#include <QImage>
#include <QPixmap>
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libswscale/swscale.h>
#include <libswscale/swscale.h>
}
#include "framebuffer.h"


class VideoDisplay : public QThread {
    Q_OBJECT
public:
    VideoDisplay(QLabel *videoLabel, FrameBuffer &frameBuffer,  AVFormatContext *decode_fmt_ctx,AVCodecContext *decoder_ctx);
    ~VideoDisplay();
    void run() override;
    void stop();
//    void startSave();
    
    void initEncodec();
public slots:
    void startSave(const QString  &filename);
    void stopSave();
signals:
    void frameReady(const QImage &image);


private:
    QLabel *videoLabel;
    FrameBuffer &frameBuffer;
    bool stopFlag;
    //保存视频的相关变量
    int pts;
    QString filename;
    AVFormatContext *encode_fmt_ctx;
    AVCodecContext *encode_ctx;
    AVFormatContext *decode_fmt_ctx;
    AVCodecContext *decoder_ctx;


    AVStream *video_stream;
    AVCodec *encodec;
    AVFrame *enframe;
    AVPacket *enpkt;
    struct SwsContext *ensws_ctx;
    bool saveFlag;//开始保存的标志位
    bool stopSaveFlag;//停止保存的标志位
};

#endif // VIDEODISPLAY_H
