#include "videorecoder.h"
#include "v4l2_example.h"
#include <QBuffer>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QQueue>
VideoRecoder::VideoRecoder(QObject *parent) :
    QThread(parent)
{

}
VideoRecoder::VideoRecoder(RealTimeShow *realTimeShow){
    this->realTimeShow = realTimeShow;
    this->framec = 0;
    this->prefixNew = 0;
    this->prefix = 0;
    this->isRecord = false;
}

VideoRecoder::~VideoRecoder(){

}
void VideoRecoder::startRecoder() {
    this->start();
}

void VideoRecoder::run(){
    QQueue<QPixmap> queue;
    while(true) {
        msleep(10); //线程休眠10ms，没10ms检测一次是否需要进行录制
        if(this->isRecord == true) {
            QPixmap pix;
            QByteArray aa;
            // 获得LOCK才能访问buffers，以防止
            // 摄像头在采集的时候对未采集完整的数据进行处理，进行数据的同步。
            this->realTimeShow->LOCK->lock();
            aa.append((const char *) buffers[0].start,IMAGEWIDTH*IMAGEHEIGHT);
            this->realTimeShow->LOCK->unlock();

            // 将数据先保存到缓存
            pix.loadFromData(aa);

            // 将图像保存转换为Mat1，进行图像压缩Mat2


            // 对Mat2进行后续操作 




            queue.push_back(pix);
            qDebug() << "video recording frame " << framec;
            framec ++;
            if (framec > 48 ) {
                this->isRecord = false;
            }

        } else if(framec > 0) {
            // 当停止录制时，如果有未保存的图像再缓存中，对图像进行保存
            qDebug() << "going to store pictures in nandflash... " ;
            framec = 0;
            while(!queue.isEmpty()) {
                QByteArray qba; 
                QBuffer bf(&qba);
                QPixmap pix = queue.front();
                pix.save(&bf,"jpg");
                queue.pop_front(); // 从队列头部删除一张图片

                // 命名图像
                char *name = NULL;
                QString str = QString("./img/image%1_%2.jpg").arg(prefix).arg(framec);
                QByteArray str_ba = str.toLatin1();
                name = str_ba.data();

                // 保存图像
                QFile fimg(name);fimg.open(QFile::WriteOnly);
                fimg.write(qba);fimg.close();
                framec++;
                qDebug() << "storing frame ... " << framec ;
            }
            qDebug() << "... ok ok ok ok ok ...";
            prefixNew = prefix;
            prefix++;framec = 0;
            // 全部保存完标志复位
            this->isRecord = false;
        }
    }
}

