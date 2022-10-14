#include "recthread.h"
#include <QApplication>
#include <QDateTime>

#define QUEUE_SIZE 64
#define BUFFER_LENGTH 512
#define TIMEOUT 1000

#define SAVE_FILE_PATH (QApplication::applicationDirPath()+"/"+QDateTime::currentDateTime().toString("yyyy-MM-dd-hh-mm-ss")+".txt")

// CyUSB
LONG inLen = BUFFER_LENGTH;
PUCHAR buffer[QUEUE_SIZE];
PUCHAR inContext[QUEUE_SIZE];
OVERLAPPED inOvLap[QUEUE_SIZE];

// data
UCHAR saveBuffer[BUFFER_LENGTH];
QVector<QString> valueList;
QFile *file;
QTextStream *fileOut;

const double V2_23 = (1<<23);

RecThread::RecThread(QObject *parent)
    : QThread{parent}
{

}

RecThread::~RecThread()
{
    if (saveToDisk)
    {
        file->close();
    }
}

void RecThread::init(CCyBulkEndPoint *outBulkEndPt, bool isSave)
{
    ept = outBulkEndPt;
    saveToDisk = isSave;
}

void RecThread::run()
{
    // savefile
    if (saveToDisk)
    {
        file = new QFile(SAVE_FILE_PATH);
        file->open(QIODevice::Append|QIODevice::Text);
        fileOut = new QTextStream(file);
    }

    // CyUSB
    ept->TimeOut = TIMEOUT;
    ept->SetXferSize(BUFFER_LENGTH);
    for (int i = 0; i < QUEUE_SIZE; ++ i)
    {
        buffer[i] = new UCHAR[BUFFER_LENGTH];
        memset(buffer[i], 0, BUFFER_LENGTH);
        inOvLap[i].hEvent = CreateEvent(NULL, false, false, L"CYUSB_IN");
    }

    for (int i = 0; i < QUEUE_SIZE; ++ i)
        inContext[i] = ept->BeginDataXfer(buffer[i], inLen, &inOvLap[i]);

    // mainloop
    int tcount = 0, cur = -1, u = 0, p;
    int tempData1, tempData2;
    QString lineData;

    while (true)
    {
        QApplication::processEvents();
        ept->WaitForXfer(&inOvLap[u], ept->TimeOut);
        ept->FinishDataXfer(buffer[u], inLen, &inOvLap[u], inContext[u]);
        p = 0;
        while (p < BUFFER_LENGTH)
        {
            if (cur == -1)
            {
                cur = 0;
                while(p+75 < BUFFER_LENGTH)
                {
                    if (buffer[u][p]==0xEB && buffer[u][p+1]==0xEB && buffer[u][p+74]==0x90 && buffer[u][p+75]==0x90) break;
                    p ++;
                }
            }
            while(p < BUFFER_LENGTH)
            {
                saveBuffer[cur ++] = buffer[u][p ++];
                if (cur == 76)
                {
                    if (saveBuffer[0] != 0xEB || saveBuffer[1] != 0xEB || saveBuffer[74] != 0x90 || saveBuffer[75] != 0x90)
                    {
                        cur = -1;
                        break;
                    }
                    cur = 0;
                    lineData = "";
                    if (tcount == 0) valueList.clear();
                    for (int k = 0; k < 12; ++ k)
                    {
                        tempData1 = ((int)saveBuffer[2+6*k]<<16)+((int)saveBuffer[2+6*k+1]<<8)+(int)saveBuffer[2+6*k+2];
                        tempData2 = ((int)saveBuffer[2+6*k+3]<<16)+((int)saveBuffer[2+6*k+4]<<8)+(int)saveBuffer[2+6*k+5];
                        if (tempData1&(1<<23)){
                            tempData1 = (tempData1^0x7FFFFF)+1;
                            tempData1 &= 0x7FFFFF;
                            tempData1 = -tempData1;
                        }
                        if (tempData2&(1<<23)){
                            tempData2 = (tempData2^0x7FFFFF)+1;
                            tempData2 &= 0x7FFFFF;
                            tempData2 = -tempData2;
                        }
                        if (saveToDisk)
                        {
                            lineData += QString::number((double(tempData1)*(3.3/V2_23)-2.048)/25, 'f', 10)+" ";
                            lineData += QString::number((double(tempData2)*(3.3/V2_23)*570)/470, 'f', 10)+" ";
                        }
                        if (tcount == 0)
                        {
                            valueList.append(QString::number((double(tempData1)*(3.3/V2_23)-2.048)/25, 'f', 10));
                            valueList.append(QString::number((double(tempData2)*(3.3/V2_23)*570)/470, 'f', 10));
                        }
                    }
                    if (tcount == 0)
                    {
                        tcount = 1000;
                        emit refreashData(valueList);
                    }
                    tcount --;
                    if (saveToDisk) *fileOut << lineData+"\n";
                }
            }
        }

        inContext[u] = ept->BeginDataXfer(buffer[u], inLen, &inOvLap[u]);

        u ++;
        if (u == QUEUE_SIZE) u = 0;
    }
}
