#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <queue>
#include <mutex>
#include<QThread>
#include<QDebug>
#include <condition_variable>
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
QT_BEGIN_NAMESPACE
#include <mutex>
#include <condition_variable>
#include<thread>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include "framebuffer.h"
#include "videodecoder.h"
#include "videosaver.h"
#include "videodisplay.h"
#include<string>

namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlay();
    void onSave();
    void onStop();
    void onStopSave();
    void displayFrame(const QImage &image);
//    void on_connect_clicked();

//    void on_disconnect_clicked();

private:
    QLabel *videoLabel;
    FrameBuffer frameBuffer;
    VideoDecoder *videoDecoder;
    VideoSaver *videoSaver;
    VideoDisplay *videoDisplay;
    bool mStop;
    Ui::MainWindow *ui;
    int mVideoWidth;//视频宽度
    int mVideoHeight;//视频高度
};











#endif // MAINWINDOW_H
