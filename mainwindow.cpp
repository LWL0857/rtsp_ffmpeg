#include "mainwindow.h"
#include "ui_mainwindow.h"
extern "C"{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}
#include <iostream>
#include <QTime>
using namespace std;
//int avformat_close_input2(AVFormatContext *&s, AVDictionary *opts) {
//    if (!s) {
//        return 0;
//    }

//    int ret = 0;
//    RTSPState *rt = s->priv_data;
//    if (rt && rt->rtsp_hd) {
//        ret = ff_rtsp_send_cmd(s, "TEARDOWN", s->filename, opts);
//    }
//    avformat_close_input(&s);
//    return ret;
//}
//Delay for some msec
void Delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < dieTime){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), frameBuffer(10), videoDecoder(nullptr), videoSaver(nullptr), videoDisplay(nullptr), ui(new Ui::MainWindow) {
    ui->setupUi(this);


    connect(ui->playButton, &QPushButton::clicked, this, &MainWindow::onPlay);
    connect(ui->saveButton, &QPushButton::clicked, this, &MainWindow::onSave);
    connect(ui->stopButton, &QPushButton::clicked, this, &MainWindow::onStop);
    connect(ui->stopsaveButton, &QPushButton::clicked, this, &MainWindow::onStopSave);
 
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onPlay()
{
    string ipStr_=ui->ip->text().toStdString();
    string portStr_=ui->port->text().toStdString();
    string pwdStr_=ui->pwd->text().toStdString();
    string inputVideoPath_="rtsp://"+ipStr_+":"+portStr_+"/"+pwdStr_;
    QString url = QString::fromStdString(inputVideoPath_);
    videoDecoder = new VideoDecoder(url, frameBuffer);


    videoDisplay = new VideoDisplay(ui->videoLabel1, frameBuffer,videoDecoder);\
    connect(videoDisplay, &VideoDisplay::frameReady, this, &MainWindow::displayFrame);
    connect(this,&MainWindow::saveFilename,videoDisplay,&VideoDisplay::startSave);
    connect(this,&MainWindow::stopSave,videoDisplay,&VideoDisplay::stopSave);

    videoDecoder->start();
    videoDisplay->start();

}
void MainWindow::onStop()
{
    if (videoDecoder) {
        videoDecoder->stop();
        videoDecoder->wait();
        delete videoDecoder;
        videoDecoder = nullptr;
//        int avformat_close_input2(AVFormatContext *&s, AVDictionary *opts)
    }

    if (videoDisplay) {
        videoDisplay->stop();
        videoDisplay->wait();
        delete videoDisplay;
        videoDisplay = nullptr;
    }

}
void MainWindow::onSave()
{

    string outputVideoPath_=QDir::currentPath().toStdString() + "/video/" +
                       QDateTime::currentDateTime().toString("yy.MM.dd").toStdString() +
                       "-" +
                       QDateTime::currentDateTime().toString("hh.mm.ss").toStdString() +
                       ".avi";
    std::cout<<outputVideoPath_<<std::endl;
    ui->saveVideoPath->setText(QString::fromStdString(outputVideoPath_));
    QString qFilename = QString::fromStdString(outputVideoPath_);


    if (videoDecoder) {
        if(videoDisplay)
        {
              emit saveFilename(qFilename);
        }
    }

}
void MainWindow::onStopSave()
{
    if (videoDisplay) {
        emit stopSave();
    }

}
void MainWindow::displayFrame(const QImage &image)
{
    ui->videoLabel1->setPixmap(QPixmap::fromImage(image));

}
