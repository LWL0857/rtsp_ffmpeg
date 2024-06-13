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
#include "videodecoder.h"

class VideoDisplay : public QThread {
    Q_OBJECT
public:
    VideoDisplay(QLabel *videoLabel, FrameBuffer &frameBuffer,VideoDecoder *videoDecoder);
    ~VideoDisplay();
    void run() override;
    void stop();
    void initEncodec();
    
public slots:
    void startSave(const QString  &filename);
    void stopSave();
signals:
    void frameReady(const QImage &image);


private:
    QLabel *videoLabel;
    FrameBuffer &frameBuffer;

    //保存视频的相关变量
    int video_stream_index;
    VideoDecoder *videoDecoder;
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
      bool stopFlag;
};

#endif // VIDEODISPLAY_H
