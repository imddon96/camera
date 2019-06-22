#ifndef VIDEORECODER_H
#define VIDEORECODER_H
#include "realtimeshow.h"
#include <QThread>
#include <QPixmap>

class VideoRecoder : public QThread
{
    Q_OBJECT
public:
    explicit VideoRecoder(QObject *parent = 0);
    ~VideoRecoder();
    VideoRecoder(RealTimeShow *realTimeShow);
    void startRecoder();

    // 图像名称格式为image[prefix]_[framec].jpg
    // 当prefix为0，framec为0，被保存的图片就为image0_0.jpg
    int prefix;
    int framec;
    int prefixNew;
    volatile bool isRecord; // VideoRecoder需要录制或者在录制的标志
signals:

public slots:
protected:
    void run();
private:
    RealTimeShow *realTimeShow;
};

#endif // VIDEORECODER_H
