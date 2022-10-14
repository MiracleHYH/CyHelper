#include "loopsend.h"
#include "qdebug.h"

LoopSend::LoopSend(QObject *parent)
    : QObject{parent}
{
    timer = nullptr;
    loopData = 0;
    BUFFER_LENGTH = 64;
    outOvLap.hEvent = CreateEvent(NULL, false, false, L"CYUSB_OUT");
}

LoopSend::~LoopSend()
{
    deleteTimer();
    CloseHandle(outOvLap.hEvent);
}

void LoopSend::createTimer(CCyBulkEndPoint *bulkEPt, int sendDuration)
{
    qDebug() << "开始循环发送测试数据";

    loopData = 0;
    ept = bulkEPt;
    ept->SetXferSize(BUFFER_LENGTH);

    timer = new QTimer();
    timer->setInterval(sendDuration);
    connect(timer, &QTimer::timeout, this, &LoopSend::sendData);
    timer->start();

//    sendData();
}

void LoopSend::sendData()
{
//    qDebug() << "sendData";
    buffer[0] = buffer[1] = 0xEE;
    buffer[62] = buffer[63] = 0x91;
    for (int i = 2; i < 62; ++ i)
    {
        buffer[i] = loopData ++;
//        qDebug() << buffer[i];
    }

    outContext = ept->BeginDataXfer(buffer, BUFFER_LENGTH, &outOvLap);
    ept->WaitForXfer(&outOvLap, 100);
    qDebug() << ept->FinishDataXfer(buffer, BUFFER_LENGTH, &outOvLap, outContext);
}

void LoopSend::deleteTimer()
{
    if (timer)
    {
        timer->stop();
        timer->deleteLater();
    }
}
