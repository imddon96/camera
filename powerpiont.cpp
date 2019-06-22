#include "powerpiont.h"
#include <QBuffer>
#include <QFile>
#include <QString>
#include <QDebug>

PowerPiont::PowerPiont(QObject *parent) :
    QThread(parent)
{
}

PowerPiont::PowerPiont(VideoRecoder *videoRecoder,RealTimeShow *realTimeShow){
    this->videoRecoder = videoRecoder;
    this->realTimeShow = realTimeShow;
}

void PowerPiont::startPlay(){
    this->start();
}

void PowerPiont::run(){
    int i = 0;
    while( true ){
        QPixmap pix;
        QString sss = QString("./img/image%1_%2.jpg")
                .arg(videoRecoder->prefixNew)
                .arg(i++);
        char *name = NULL;
        QByteArray str_ba = sss.toLatin1();
        name = str_ba.data();
        qDebug() << name;
        pix.load(name);
        if (pix.isNull() && i > 10) {
            qDebug() << "no more pictures";
            break;
        }
        msleep(20);
        pix = pix.scaled(320,240);
        emit sig_GetOnePicture(pix);
    }
    this->realTimeShow->isReplay = false;
}

