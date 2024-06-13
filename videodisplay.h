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
    void startSave();
    void stopSave();
    void initEncodec();

signals:
    void frameReady(const QImage &image);


private:
    QLabel *videoLabel;
    FrameBuffer &frameBuffer;
    bool stopFlag;
    bool saveFlag;
    bool stopSaveFlag
};

#endif // VIDEODISPLAY_H
