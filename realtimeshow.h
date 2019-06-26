#ifndef REALTIMESHOW_H
#define REALTIMESHOW_H

#include <QThread>
#include <QPixmap>
#include <QMutex>
#include <QString>

class RealTimeShow : public QThread
{
    Q_OBJECT
public:
    explicit RealTimeShow(QObject *parent = 0);
    ~RealTimeShow();
    void RTShow();

    volatile bool isReplay;
    volatile bool isContinue;

    QMutex *LOCK;

signals:
    void sig_GetOneRealTimeFrame(QPixmap);
    void sig_showlog(QString);
    void sig_SendByteArray(QByteArray);
public slots:

protected:
    void run();

};
#endif // REALTIMESHOW_H
