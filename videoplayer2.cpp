#include "videoplayer2.h"
extern "C"
{
    #include "libavcodec/avcodec.h"
    #include "libavformat/avformat.h"
    #include "libavutil/pixfmt.h"
    #include "libswscale/swscale.h"
}
#include <QDebug>
#include <stdio.h>
#include <iostream>
#include <QPixmap>
using namespace std;
VideoPlayer2::VideoPlayer2(QObject *parent) :
    QThread(parent)
{
}
VideoPlayer2::VideoPlayer2(const char* fileName){
    this->fileName = fileName;
}

VideoPlayer2::~VideoPlayer2(){

}
void VideoPlayer2::startPlay()
{
    ///调用 QThread 的start函数 将会自动执行下面的run函数 run函数是一个新的线程
    ///
    ///
    this->start();

}
void VideoPlayer2::run()
{

    AVFormatContext *pFormatCtx;
    AVCodecContext *pCodecCtx;
    AVCodec *pCodec;
    AVFrame *pFrame, *pFrameRGB;
    AVPacket *packet;
    uint8_t *out_buffer;

    static struct SwsContext *img_convert_ctx;

    int videoStream, i, numBytes;
    int ret, got_picture;

    av_register_all();         //初始化FFMPEG  调用了这个才能正常适用编码器和解码器

    pFormatCtx = avformat_alloc_context();
    /// "/home/defu/wudd/tt30.mp4"
    if (avformat_open_input(&pFormatCtx, this->fileName, NULL, NULL) != 0) {
        printf("can't open the file. \n");
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) != 0) {
        printf("Could't find stream infomation.\n");
        return;
    }

    videoStream = -1;

    ///只处理视频流  音频流不管
    for (i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
        }
    }

    ///如果videoStream为-1 说明没有找到视频流
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return;
    }

    ///查找解码器
    pCodecCtx = pFormatCtx->streams[videoStream]->codec;
    pCodec = avcodec_find_decoder(pCodecCtx->codec_id);

    if (pCodec == NULL) {
        printf("Codec not found.\n");
        return;
    }

    if (avcodec_open2(pCodecCtx,pCodec,NULL) < 0) {
        printf("Could not open codec \n");
        return;
    }

    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();

    ///将解码后的YUV数据转换成RGB32
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
            pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
            AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    numBytes = avpicture_get_size(AV_PIX_FMT_RGB32, pCodecCtx->width,pCodecCtx->height);

    out_buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));

    avpicture_fill((AVPicture *) pFrameRGB, out_buffer, AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height);

    int y_size = pCodecCtx->width * pCodecCtx->height;

    packet = (AVPacket *) malloc(sizeof(AVPacket)); //分配一个packet
    av_new_packet(packet, y_size); //分配packet的数据

    while(1) {
        if (av_read_frame(pFormatCtx, packet) < 0)
        {
            continue; //这里认为视频读取完了
        }

        if (packet->stream_index != videoStream)
            continue;

        avcodec_send_packet(pCodecCtx,packet);

        if (avcodec_receive_frame(pCodecCtx,pFrame) < 0)
            continue;

        if (got_picture) {
            sws_scale(img_convert_ctx,
                    (uint8_t const * const *) pFrame->data,
                    pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                    pFrameRGB->linesize);
            QImage tmpImg((uchar *)out_buffer,pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
            QImage image = tmpImg.copy();

            emit this->sig_GetOneFrame(image);
        }
         av_free_packet(packet);
    }

    av_free(out_buffer);
    av_free(pFrameRGB);
    avcodec_close(pCodecCtx);
    avformat_close_input(&pFormatCtx);
}
