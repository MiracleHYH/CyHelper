#ifndef SENDTHREAD_H
#define SENDTHREAD_H

#include <QThread>

class CCyBulkEndPoint;

class SendThread : public QThread
{
    Q_OBJECT
public:
    explicit SendThread(QObject *parent = nullptr);
    void init(CCyBulkEndPoint *_ept, unsigned long _waitTime, long _bufferLength, QString _bufferText);

signals:


    // QThread interface
protected:
    void run();
private:
    CCyBulkEndPoint *ept; // bulk向USB传输的EndPoint指针
    unsigned long waitTime;
    long bufferLength; // Buffer长度
    QString bufferText; // BufferData

};

#endif // SENDTHREAD_H
