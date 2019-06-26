#ifndef VIDEOPLAYER2_H
#define VIDEOPLAYER2_H

#include <QThread>
#include <QImage>
class VideoPlayer2 : public QThread
{
    Q_OBJECT
public:
    explicit VideoPlayer2(QObject *parent = 0);
    VideoPlayer2(const char* fileName);
    ~VideoPlayer2();
    void startPlay();

signals:
    void sig_GetOneFrame(QImage); //每获取到一帧图像 就发送此信号
public slots:

protected:
    void run();
private:
    const char* fileName;
};

#endif // VIDEOPLAYER2_H
