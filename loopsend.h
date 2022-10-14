#ifndef LOOPSEND_H
#define LOOPSEND_H

#include <QObject>
#include <QTimer>

#include <qt_windows.h>
#include <CyAPI.h>

#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

class LoopSend : public QObject
{
    Q_OBJECT

public:
    explicit LoopSend(QObject *parent = nullptr);
    ~LoopSend();

public:
    void createTimer(CCyBulkEndPoint *bulkEPt, int sendDuration);
    void sendData();
    void deleteTimer();

signals:

private:
    QTimer *timer;
    unsigned char loopData;
    CCyBulkEndPoint *ept;
    unsigned char buffer[64];
    OVERLAPPED outOvLap;
    UCHAR *outContext;
    long BUFFER_LENGTH;
};

#endif // LOOPSEND_H
