#ifndef REALTIMESHOW_H
#define REALTIMESHOW_H

#include <QThread>
#include <QPixmap>
#include <QMutex>

class RealTimeShow : public QThread
{
    Q_OBJECT
public:
    explicit RealTimeShow(QObject *parent = 0);
    ~RealTimeShow();
    void RTShow();

    volatile bool isReplay;
    QMutex *LOCK;

signals:
    void sig_GetOneRealTimeFrame(QPixmap);
public slots:

protected:
    void run();

};
#endif // REALTIMESHOW_H
