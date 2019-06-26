#include "widget.h"
#include "ui_widget.h"
#include "v4l2_example.h"
#include <iostream>
#include <QThread>
#include <QString>
#include <QBuffer>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QMessageBox>

//#include <opencv2/imgproc/imgproc.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/video/video.hpp>
//#include <opencv2/opencv.hpp>
//#include "videoplayer2.h"

#include "realtimeshow.h"
#include "videorecoder.h"
#include <QFont>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    QString theme = "background-color:rgb(237,59,88);color:rgb(255,255,255);font-size:20px;";

    ui->open->setStyleSheet(theme);
    ui->take->setStyleSheet(theme);
    ui->close->setStyleSheet(theme);
    ui->record->setStyleSheet(theme);
    ui->replay->setStyleSheet(theme);



    connect(ui->open, SIGNAL(clicked()), this, SLOT(open()));
    connect(ui->take, SIGNAL(released()), this, SLOT(take()));
    connect(ui->close, SIGNAL(clicked()), this, SLOT(close()));
    connect(ui->record, SIGNAL(clicked()), this, SLOT(record()));
    connect(ui->replay, SIGNAL(clicked()),this,SLOT(ppt()));

    powerPiont = NULL;
    videoPlayer2 = NULL;
    realTimeShow = new RealTimeShow();
    videoRecoder = new VideoRecoder(realTimeShow);
    connect(realTimeShow,SIGNAL(sig_GetOneRealTimeFrame(QPixmap)),this,SLOT(slotGetOneRealTimePixmap(QPixmap)));
    connect(realTimeShow,SIGNAL(sig_showlog(QString)),this,SLOT(showlog(QString)));
    connect(realTimeShow,SIGNAL(sig_SendByteArray(QByteArray)),this,SLOT(sendByteArray(QByteArray)));
    connect(videoRecoder,SIGNAL(sig_showlog(QString)),this,SLOT(showlog(QString)));

    client = new QTcpSocket();

}
void Widget::showlog(QString str){

    ui->log->setText(str);
}

// 打开摄像头并启动实时显示
void Widget::open(){
    ui->close->setVisible(true);
    ui->take->setVisible(true);
    ui->open->setVisible(false);
    ui->label1->setVisible(true);


    client->connectToHost("192.168.1.20",10086);
    // 打开摄像头
    qDebug() << "open camera";
    v4l2_init();
    qDebug() << "v4l2 init...";
    v4l2_mem_ops();
    qDebug() << "v4l2 memory opreations...";

    showlog(QString("camera open successfully ... "));

    // 启动实时显示
    realTimeShow->RTShow();
    // 启动录制任务，但不在录制，当按下record按钮时，会进行录制。
    videoRecoder->startRecoder();


}

// 更新录制标志
void Widget::record(){
    bool new_state = !this->videoRecoder->isRecord;
    this->videoRecoder->isRecord = new_state;
}

// 当界面上的play按钮按下时触发这个槽函数
// 将启动powerPiont线程，并在线程中执行图片回放功能。
void Widget::ppt(){

    if(this->realTimeShow->isReplay == true)return;
    this->realTimeShow->isReplay = true;
    powerPiont = new PowerPiont(videoRecoder,realTimeShow);
    connect(powerPiont,SIGNAL(sig_GetOnePicture(QPixmap)),this,SLOT(slotGetOneRealTimePixmap(QPixmap)));
    connect(powerPiont,SIGNAL(sig_showlog(QString)),this,SLOT(showlog(QString)));
    // 开启这个powerPiont线程
    powerPiont->startPlay();
}

Widget::~Widget() {
    delete ui;
}


// 当界面上的take按钮被按下时触发这个槽函数
// 直接在该widget线程里面保存并显示缩略图到label2上
void Widget::take(){
//      videoPlayer2 = new VideoPlayer2("/video/image2video.avi");
//      this->realTimeShow->isReplay = false;
//      connect(videoPlayer2,SIGNAL(sig_GetOneFrame(QImage)),this,SLOT(slotImage(QImage)));
//      videoPlayer2->startPlay();


    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date =current_date_time.toString("yyyy.MM.dd hh:mm:ss");

    qDebug() << "Taking a picture...";

    // 显示缩略图到label2上
    QPixmap pix;
    QByteArray aa ;
    aa.append((const char *) gInterfaceBuf,gLength);
    pix.loadFromData(aa);
    pix.scaled(50,50);
    ui->label2->setVisible(true);
    ui->label2->setPixmap(pix);

    // 生产被保存图像的名字
    char *name = NULL;
    QString str = "./img/";
    str.append(current_date);
    str.append(".jpg");
    QByteArray str_ba = str.toLatin1();
    name = str_ba.data();
    showlog(str);

    // 使用pix的save函数将采集的图片保存到Qbuffer缓存中
    QByteArray qba;
    QBuffer bf(&qba);
    if (!pix.save(&bf,"jpg")) {
        qDebug() << "picture save failed, please check.";
        return;
    }

    // 使用QFile将缓存在Qbuffer中的图像保存到磁盘中
    QFile fimg(name);
    fimg.open(QFile::WriteOnly);
    fimg.write(qba);
    fimg.close();

    int length = 0;
    if ( (length = client->write(qba)) == -1) {
        client->close();
        showlog(QString("reconnect to server!"));
        client->connectToHost("192.168.1.20",10086);
    } else {
        showlog(QString("send success! %1/%2").arg(length).arg(qba.size()));
    }


}

// 关闭资源，释放申请的内存
void Widget::close(){
//    showlog(QString("exit program"));
    ui->take->setVisible(false);
    ui->open->setVisible(true);
    ui->label1->setVisible(false);
    ui->close->setVisible(false);

    videoRecoder->isContinue = false;
    videoRecoder->wait();

    realTimeShow->isContinue = false;
    realTimeShow->wait();

    if ( powerPiont != NULL) {
        powerPiont->isContinue = false;
        powerPiont->wait();
    }

    v4l2_release();
    exit(0);
}
void Widget::sendByteArray(QByteArray qba) {
    if(client->isValid()) {
        client->connectToHost("192.168.1.20",10086);
        showlog(QString("reconnet to the server"));
        return ;
    }
    int length = 0;
    if ( (length = client->write(qba)) == -1) {
        showlog(QString("send faild!"));
    } else {
        showlog(QString("send success! %1").arg(length));
    }
}

// 在label1上显示一张图片的槽函数
void Widget::slotGetOneRealTimePixmap(QPixmap pix) {

    ui->label1->setPixmap(pix);
}
void Widget::slotImage(QImage image) {
    QPixmap pix;
    ui->label1->setPixmap(pix.fromImage(image));
}
