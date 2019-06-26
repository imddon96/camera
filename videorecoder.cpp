#include "videorecoder.h"
#include "v4l2_example.h"
#include <QBuffer>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QQueue>


#include <opencv2/opencv.hpp>
#include <qttoopencv.h>

using namespace QtToOpencv;
using namespace cv;

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
    this->isContinue = true;
}

VideoRecoder::~VideoRecoder(){

}
void VideoRecoder::startRecoder() {
    this->start();
}

void VideoRecoder::run(){
    QQueue<Mat> queue;
    while(true) {
        msleep(5); //线程休眠10ms，没10ms检测一次是否需要进行录制 10 --
        if (!isContinue) {
            break;
        }
        if(this->isRecord == true) {
            QPixmap pix;
            QByteArray aa;
            // 获得LOCK才能访问buffers，以防止
            // 摄像头在采集的时候对未采集完整的数据进行处理，进行数据的同步。
            this->realTimeShow->LOCK->lock();
            aa.append((const char *) gInterfaceBuf,gLength);
            qDebug()<<buffers[0].length;
            this->realTimeShow->LOCK->unlock();

            // 将数据先保存到缓存
            pix.loadFromData(aa);

            // 将图像保存转换为Mat1，进行图像压缩Mat2
            Mat img = ImageConversion::QPixmapToCvMat(pix);
            resize(img,img,Size(160,120),0,0);

            // 对Mat2进行后续操作           

            queue.push_back(img);
            qDebug() << "video recording frame " << framec;
            emit sig_showlog(QString("video recording frame %1").arg(framec));
            framec ++;
            if (framec > 400 ) {
                this->isRecord = false;
            }

        } else if(framec > 0) {
            // 当停止录制时，如果有未保存的图像再缓存中，对图像进行保存
            qDebug() << "going to store pictures in nandflash... " ;
            emit sig_showlog(QString("going to store pictures in nandflash... ") );

            framec = 0;
            while(!queue.isEmpty()) {
                // 命名图像
                char *name = NULL;
                QString str = QString("./img/image%1_%2.jpg").arg(prefix).arg(framec);
                QByteArray str_ba = str.toLatin1();
                name = str_ba.data();

                // 保存图像
                Mat img = queue.front();
                queue.pop_front();  // 从队列头部删除一张图片
                imwrite(name,img);
                framec++;
                qDebug() << "storing frame ... " << framec ;
                emit sig_showlog(QString("storing frame ... %1").arg(framec));
            }
            qDebug() << "... ok ok ok ok ok ...";
            emit sig_showlog(QString("... ok ok ok ok ok ..."));
            prefixNew = prefix;
            prefix++;framec = 0;
            // 全部保存完标志复位
            this->isRecord = false;
        }
    }
    qDebug() << "Exit video recoder thread.";
}

