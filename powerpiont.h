#ifndef POWERPIONT_H
#define POWERPIONT_H

#include <QThread>
#include <QPixmap>
#include <videorecoder.h>
#include <realtimeshow.h>

class PowerPiont : public QThread
{
    Q_OBJECT
public:
    explicit PowerPiont(QObject *parent = 0);
    PowerPiont(VideoRecoder *videoRecoder,RealTimeShow *realTimeShow);
    void startPlay();
    volatile bool isContinue;
signals:
    void sig_GetOnePicture(QPixmap);
    void sig_showlog(QString);
public slots:

protected:
    void run();
    VideoRecoder *videoRecoder;
    RealTimeShow *realTimeShow;
};

#endif // POWERPIONT_H
