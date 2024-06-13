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


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), frameBuffer(10), videoDecoder(nullptr), videoSaver(nullptr), videoDisplay(nullptr), ui(new Ui::MainWindow) {
    ui->setupUi(this);
//    QVBoxLayout *layout = new QVBoxLayout;
//    QWidget *widget = new QWidget(this);

//    videoLabel = new QLabel(this);
//    QPushButton *playButton = new QPushButton("Play", this);
//    QPushButton *saveButton = new QPushButton("Save", this);
//    QPushButton *stopButton = new QPushButton("Stop", this);

//    layout->addWidget(videoLabel);
//    layout->addWidget(playButton);
//    layout->addWidget(saveButton);
//    layout->addWidget(stopButton);
//    widget->setLayout(layout);
//    setCentralWidget(widget);

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
    videoDisplay = new VideoDisplay(ui->videoLabel1, frameBuffer,videoDecoder->getFormatContext(),videoDecoder->getCodecContext());

    connect(videoDisplay, &VideoDisplay::frameReady, this, &MainWindow::displayFrame);
    connect(this,&MainWindow::saveFilename,videoDisplay,&VideoDisplay::startSave);
    connect(this,&MainWindow::stopSave,videoDisplay,&VideoDisplay::stopSave);

    videoDecoder->start();
    videoDisplay->start();

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
        emit saveFilename(qFilename);
        // videoSaver = new VideoSaver(frameBuffer, videoDecoder->getCodecContext(),videoDecoder->getFormatContext(),outputVideoPath_.c_str());
        // connect(videoDisplay, &VideoDisplay::frameReady, videoSaver, &videoSaver::getFrame);
        // videoSaver->start();
    }

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
    //     if (videoSaver) {
    //     videoSaver->stop();
    //     videoSaver->wait();
    //     delete videoSaver;
    //     videoSaver = nullptr;
    // }
    if (videoDisplay) {
        videoDisplay->stop();
        videoDisplay->wait();
        delete videoDisplay;
        videoDisplay = nullptr;
    }

}

void MainWindow::onStopSave()
{
    if (videoDisplay) {
        emit stopSave();
    }
    // if (videoSaver) {
    //     videoSaver->stop();
    //     videoSaver->wait();
    //     delete videoSaver;
    //     videoSaver = nullptr;
    // }

}
void MainWindow::displayFrame(const QImage &image)
{
    ui->videoLabel1->setPixmap(QPixmap::fromImage(image));

}
//Delay for some msec
void Delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < dieTime){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

//void MainWindow::on_connect_clicked()
//{
//    AVFormatContext *pFormatCtx = NULL;
//    AVCodecContext *pCodecCtx = NULL;
//    const AVCodec *pCodec = NULL;
//    AVFrame *pFrame,*pFrameRGB;
//    AVPacket *packet;
//    struct SwsContext *img_convert_ctx;

//    unsigned char *out_buffer;
//    int i,videoIndex;
//    int ret;
//    char errors[1024] = "";
//    string ipStr_=ui->ip->text().toStdString();
//    string portStr_=ui->port->text().toStdString();
//    string pwdStr_=ui->pwd->text().toStdString();
//    string inputVideoPath_="rtsp://"+ipStr_+":"+portStr_+"/"+pwdStr_;
//    //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
//    pFormatCtx = avformat_alloc_context();  //init FormatContext
//    //初始化FFmpeg网络模块
//    avformat_network_init();    //init FFmpeg network


//    //open Media File
//    ret = avformat_open_input(&pFormatCtx,inputVideoPath_.c_str(),NULL,NULL);
//    if(ret != 0){
//        av_strerror(ret,errors,sizeof(errors));
//        cout <<"Failed to open video: ["<< ret << "]"<< errors << endl;
//        exit(ret);
//    }

//    //Get audio information
//    ret = avformat_find_stream_info(pFormatCtx,NULL);
//    if(ret != 0){
//        av_strerror(ret,errors,sizeof(errors));
//        cout <<"Failed to get audio info: ["<< ret << "]"<< errors << endl;
//        exit(ret);
//    }

//    videoIndex = -1;

//    ///循环查找视频中包含的流信息，直到找到视频类型的流
//    ///便将其记录下来 videoIndex
//    ///这里我们现在只处理视频流  音频流先不管他
//    for (i = 0; i < pFormatCtx->nb_streams; i++) {
//        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
//            videoIndex = i;
//        }
//    }

//    ///videoIndex-1 说明没有找到视频流
//    if (videoIndex == -1) {
//        printf("Didn't find a video stream.\n");
//        return;
//    }

//    //配置编码上下文，AVCodecContext内容
//    //1.查找解码器
//    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
//    //2.初始化上下文
//    pCodecCtx = avcodec_alloc_context3(pCodec);
//    //3.配置上下文相关参数
//    avcodec_parameters_to_context(pCodecCtx,pFormatCtx->streams[videoIndex]->codecpar);
//    //4.打开解码器
//    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
//    if(ret != 0){
//        av_strerror(ret,errors,sizeof(errors));
//        cout <<"Failed to open Codec Context: ["<< ret << "]"<< errors << endl;
//        exit(ret);
//    }

//    //初始化视频帧
//    pFrame = av_frame_alloc();
//    pFrameRGB = av_frame_alloc();
//    //为out_buffer申请一段存储图像的内存空间
//    out_buffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32,pCodecCtx->width,pCodecCtx->height,1));
//    //实现AVFrame中像素数据和Bitmap像素数据的关联
//    av_image_fill_arrays(pFrameRGB->data,pFrameRGB->linesize, out_buffer,
//                         AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height,1);
//    //为AVPacket申请内存
//    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
//    //打印媒体信息
//    av_dump_format(pFormatCtx,0,inputVideoPath_.c_str(),0);
//    //初始化一个SwsContext
//    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                                     pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
//                                     AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

//    //设置视频label的宽高为视频宽高
//    ui->label->setGeometry(0, 0, pCodecCtx->width, pCodecCtx->height);

//    FrameBuffer frameBuffer(10); // 创建一个大小为10的缓冲区

//    //    //读取帧数据，并通过av_read_frame的返回值确认是不是还有视频帧
//    //    while(av_read_frame(pFormatCtx,packet) >=0){
//    //        //判断视频帧
//    //        if(!mStop){
//    //            if(packet->stream_index == videoIndex){
//    //                //解码视频帧
//    //                ret = avcodec_send_packet(pCodecCtx, packet);
//    //                ret = avcodec_receive_frame(pCodecCtx, pFrame);
//    //                if(ret != 0){
//    //                    av_strerror(ret,errors,sizeof(errors));
//    //                    cout <<"Failed to decode video frame: ["<< ret << "]"<< errors << endl;
//    //                }
//    //                if (ret == 0) {
//    //                    //处理图像数据
//    //                    sws_scale(img_convert_ctx,
//    //                              (const unsigned char* const*) pFrame->data,
//    //                              pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
//    //                              pFrameRGB->linesize);
//    //                    QImage img((uchar*)pFrameRGB->data[0],pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
//    //                    ui->ipc_Cam->setPixmap(QPixmap::fromImage(img));
//    //                    //释放前需要一个延时
//    //                    Delay(1);
//    //                }
//    //            }
//    //            //释放packet空间
//    //            av_packet_unref(packet);



//    //        }else{
//    //            //释放packet空间
//    //            av_packet_unref(packet);
//    //            break;
//    //        }


//    //    }

//    std::thread decodeThread([&]() {
//        while (av_read_frame(pFormatCtx, packet) >= 0) {
//            if (packet->stream_index == videoIndex) {
//                ret = avcodec_send_packet(pCodecCtx, packet);
//                if (ret < 0) {
//                    av_strerror(ret, errors, sizeof(errors));
//                    cout << "Failed to send packet: [" << ret << "]" << errors << endl;
//                    av_packet_unref(packet);
//                    continue;
//                }
//                while (ret >= 0) {
//                    ret = avcodec_receive_frame(pCodecCtx, pFrame);
//                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
//                        break;
//                    } else if (ret < 0) {
//                        av_strerror(ret, errors, sizeof(errors));
//                        cout << "Failed to receive frame: [" << ret << "]" << errors << endl;
//                        break;
//                    }
//                    // 创建一个新的 AVFrame 实例并拷贝数据
//                    AVFrame* pFrameCopy = av_frame_alloc();
//                    if (!pFrameCopy) {
//                        cout << "Failed to allocate AVFrame." << endl;
//                        break;
//                    }
//                    if (av_frame_ref(pFrameCopy, pFrame) < 0) {
//                        cout << "Failed to copy AVFrame." << endl;
//                        av_frame_free(&pFrameCopy);
//                        break;
//                    }
//                    frameBuffer.push(pFrameCopy);
//                    qDebug("push frame");
//                }
//            }
//            av_packet_unref(packet);
//            if (mStop) break;
//        }
//    });

//    // 渲染线程
//    //    std::thread renderThread([&]() {
//    while (!mStop) {
//        AVFrame* frame = frameBuffer.pop();
//        if (frame && frame->data[0] != NULL) {
//            qDebug("pop frame");

//            // 确保 img_convert_ctx, pFrame, pFrameRGB 和 pCodecCtx 是正确初始化的
//            sws_scale(img_convert_ctx,
//                      (const unsigned char* const*) frame->data,
//                      frame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
//                      pFrameRGB->linesize);

//            QImage img((uchar*)pFrameRGB->data[0], pCodecCtx->width, pCodecCtx->height, QImage::Format_RGB32);
//            cout << img.height() << endl;
//            ui->videoLabel1->setPixmap(QPixmap::fromImage(img));
//            qDebug("pop frame");

//            av_frame_free(&frame);
//        } else {
//            qDebug("frame is null or frame data is null");
//        }
//        Delay(30); // 延时，模拟30ms的帧率
//    }
//    //    });

//    decodeThread.join();
//    //    renderThread.join();




//    //close and release resource
//    av_free(out_buffer);
//    av_free(pFrameRGB);

//    sws_freeContext(img_convert_ctx);
//    avcodec_close(pCodecCtx);
//    avcodec_free_context(&pCodecCtx);
//    avformat_close_input(&pFormatCtx);
//    //    exit(0);
//}


//void MainWindow::on_disconnect_clicked()
//{
//    mStop=true;
//}

