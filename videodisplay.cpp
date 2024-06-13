#include "videodisplay.h"

VideoDisplay::VideoDisplay(QLabel *videoLabel, FrameBuffer &frameBuffer)
    : videoLabel(videoLabel), frameBuffer(frameBuffer), stopFlag(false) {}





void VideoDisplay::run() {
    AVFrame *frame = nullptr;
    SwsContext *sws_ctx = nullptr;

    while (!stopFlag) {
        frame = frameBuffer.pop();

        if (!frame) continue;
        // emit frameSaveReady(frame);
        if(saveFlag&&!stopSaveFlag)
        {
            

        }else if(!saveFlag&&stopSaveFlag){

        }



        if (!sws_ctx) {
            sws_ctx = sws_getContext(frame->width, frame->height, (AVPixelFormat)frame->format,
                                     frame->width, frame->height, AV_PIX_FMT_RGB24,
                                     SWS_BILINEAR, nullptr, nullptr, nullptr);
        }

        QImage img(frame->width, frame->height, QImage::Format_RGB888);
        uint8_t *data[1] = { img.bits() };
        int linesize[1] = { img.bytesPerLine() };

        sws_scale(sws_ctx, frame->data, frame->linesize, 0, frame->height, data, linesize);

        emit frameReady(img);
        av_frame_free(&frame);
    }

    if (sws_ctx) {
        sws_freeContext(sws_ctx);
    }
}

void VideoDisplay::stop() {
    stopFlag = true;
}
