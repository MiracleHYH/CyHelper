#include "sendthread.h"
#include "qdebug.h"

#include <qt_windows.h>
#include <CyAPI.h>

#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

SendThread::SendThread(QObject *parent)
    : QThread{parent}
{

}

void SendThread::init(CCyBulkEndPoint *_ept, unsigned long _waitTime, long _bufferLength, QString _bufferText)
{
    ept = _ept;
    waitTime = _waitTime;
    bufferLength = _bufferLength;
    bufferText = _bufferText;
}

void SendThread::run()
{
    PUCHAR buffer = new UCHAR [bufferLength];
    for (int i = 0; i < bufferLength; ++ i)
    {
        buffer[i] = bufferText.mid(i<<1, 2).toInt(nullptr, 16);
    }
    ept->SetXferSize(bufferLength);
    OVERLAPPED outOvLap;
    outOvLap.hEvent = CreateEvent(NULL, false, false, L"CYUSB_OUT");
    UCHAR *outContext = ept->BeginDataXfer(buffer, bufferLength, &outOvLap);
    ept->WaitForXfer(&outOvLap, waitTime);
    bool success = ept->FinishDataXfer(buffer, bufferLength, &outOvLap, outContext);
    CloseHandle(outOvLap.hEvent);
    delete [] buffer;
    qDebug() << "单次发送返回值" << success;
    qDebug() << "单次发送完成,释放buff空间,结束子线程.....";
}
