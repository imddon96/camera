#include "realtimeshow.h"
#include "v4l2_example.h"
#include <QDebug>
#include <QMutex>
RealTimeShow::RealTimeShow(QObject *parent) :
    QThread(parent)
{
    this->isReplay = false;
    LOCK = new QMutex();
}

RealTimeShow::~RealTimeShow(){

}

void RealTimeShow::RTShow(){
    this->start();
}

void RealTimeShow::run(){
   while(1) {
       msleep(15); // 每15ms执行一次图像采集，并将图像发送到widget显示。
       if (!isReplay){
//           qDebug() << "real time simple... in thread " << QThread::currentThreadId();
           LOCK->lock();
           v4l2_frame_process();
           LOCK->unlock();
           QPixmap pix;
           QByteArray aa ;
           aa.append((const char *) gInterfaceBuf,gLength);
           pix.loadFromData(aa);
           pix = pix.scaled(320,240);
           emit sig_GetOneRealTimeFrame(pix);
       }
   }
}
