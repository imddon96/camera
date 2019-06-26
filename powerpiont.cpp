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
    this->isContinue = true;
}

void PowerPiont::startPlay(){
    this->start();
}

void PowerPiont::run(){
    int tbf = 30;
    int i = 0;
    while( true ){

        if (!isContinue) {
            break;
        }

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
            emit sig_showlog(QString("play over"));
            break;
        }
        msleep(tbf);
        pix = pix.scaled(320,240);
        emit sig_showlog(sss.append("  >30ms"));
        emit sig_GetOnePicture(pix);
    }
    this->realTimeShow->isReplay = false;
    qDebug() << "Exit powerpoint thread.";
}

