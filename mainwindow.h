#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class USBCheckThread;
class SendThread;
class RecThread;
class CCyBulkEndPoint;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class CCyUSBDevice;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    CCyUSBDevice *USBDevice;
    USBCheckThread * usbCheckThread; // USB设备监控线程
    RecThread *recThread; // 数据接收线程
    SendThread *sendThread; // 单次异步发送线程
    QThread *loopSendThread; // 循环发送测试线程
    CCyBulkEndPoint *inEndPt, *outEndPt;
};
#endif // MAINWINDOW_H
