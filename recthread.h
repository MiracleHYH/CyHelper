#ifndef RECTHREAD_H
#define RECTHREAD_H

#include <QThread>
#include <QFile>

#include <QTextStream>

#include <qt_windows.h>
#include <CyAPI.h>

#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

class RecThread : public QThread
{
    Q_OBJECT
public:
    explicit RecThread(QObject *parent = nullptr);
    ~RecThread();
    void init(CCyBulkEndPoint *outBulkEndPt, bool isSave);

signals:
    void refreashData(QVector<QString> data);

private:
    CCyBulkEndPoint *ept;
    bool saveToDisk;

    // QThread interface
protected:
    void run();
};

#endif // RECTHREAD_H
