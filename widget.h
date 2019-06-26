#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QWidget>
#include <QTimer>
#include <QImage>
#include <QString>

#include "realtimeshow.h"
#include "videorecoder.h"
#include "powerpiont.h"
#include "videoplayer2.h"

#include <QTcpSocket>

/*extern "C"
{
    #include <ffmpeg/libavcodec/avcodec.h>
    #include <ffmpeg/libavformat/avformat.h>
    #include <ffmpeg/libswscale/swscale.h>
    #include <ffmpeg/libavdevice/avdevice.h>
    #include <ffmpeg/libavformat/version.h>
    #include <ffmpeg/libavutil/time.h>
    #include <ffmpeg/libavutil/mathematics.h>
}*/

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();


signals:
private slots:
    void open();
    void take();
    void close();
    void record();
    void ppt();
    void showlog(QString);
    void slotGetOneRealTimePixmap(QPixmap);
    void sendByteArray(QByteArray);
    void slotImage(QImage);

private :
    Ui::Widget *ui;
    RealTimeShow *realTimeShow;
    VideoRecoder *videoRecoder;
    PowerPiont *powerPiont;

    VideoPlayer2 *videoPlayer2;
    QTcpSocket *client;
};

#endif // WIDGET_H
