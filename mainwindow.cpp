#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sendthread.h"
#include "loopsend.h"
#include <QMessageBox>
#include "recthread.h"

#include <qt_windows.h>
#include <CyAPI.h>

#pragma comment(lib,"SetupAPI.lib")
#pragma comment(lib,"User32.lib")
#pragma comment(lib,"legacy_stdio_definitions.lib")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    USBDevice = new CCyUSBDevice(this);
    sendThread = new SendThread;
    loopSendThread = new QThread;
    LoopSend *loopSendWork = new LoopSend;

    // 设置并开启循环发送线程
    loopSendWork->moveToThread(loopSendThread);
    connect(loopSendThread, &QThread::finished, loopSendWork, &QObject::deleteLater);
    loopSendThread->start();

    // 打开USB按钮回调
    connect(ui->USBOpenButton, &QPushButton::clicked, this, [=]()
    {
        if (USBDevice->DeviceCount() > 0)
        {
            if (USBDevice->Open(0))
            {
                QMessageBox::information(ui->widget, "Info", "USB设备打开成功");
                inEndPt = (CCyBulkEndPoint *)USBDevice->EndPointOf(0x86);
                outEndPt = (CCyBulkEndPoint *)USBDevice->EndPointOf(0x02);
                ui->loopSendButton->setEnabled(true);
                ui->sendOnceButton->setEnabled(true);
                ui->startReceiveButton->setEnabled(true);
            }
            else
            {
                QMessageBox::critical(ui->widget, "Error", "无法打开USB设备");
            }
        }
        else
        {
            QMessageBox::critical(ui->widget, "Error", "未检测到USB设备连接,请检查USB接口和驱动");
        }
    });

    // 单次发送按钮回调
    connect(ui->sendOnceButton, &QPushButton::clicked, this, [=]()
    {
        LONG bufferLength = ui->sendBufferLength->value();
        QString bufferText = ui->sendBuffer->text();
        LONG textLength = bufferText.length();
        if (bufferLength > 0)
        {
            if (bufferLength*2 == textLength)
            {
                sendThread->init(outEndPt, 100, bufferLength, bufferText);
                sendThread->start();
            }
            else
            {
                QMessageBox::critical(ui->widget, "Error", "BUF长度与BUF内容不匹配");
            }
        }
        else
        {
            QMessageBox::critical(ui->widget, "Error", "BUF长度为0时无法发送数据");
        }
    });

    // 循环发送按钮回调
    connect(ui->loopSendButton, &QPushButton::clicked, this, [=]()
    {
        loopSendWork->createTimer(outEndPt, ui->sendDuration->value());
        ui->loopSendButton->setEnabled(false);
        ui->loopStopButton->setEnabled(true);
        ui->sendOnceButton->setEnabled(false);
    });

    // 停止发送按钮回调
    connect(ui->loopStopButton, &QPushButton::clicked, this, [=]()
    {
        loopSendWork->deleteTimer();
        ui->loopStopButton->setEnabled(false);
        ui->loopSendButton->setEnabled(true);
        ui->sendOnceButton->setEnabled(true);
    });

    // 开始接收按钮回调
    connect(ui->startReceiveButton, &QPushButton::clicked, this, [=]()
    {
        recThread = new RecThread;
        connect(recThread, &RecThread::refreashData, this, [=](QVector<QString> data)
        {
            for (int i = 0; i < 24; ++ i)
            {
                ui->originalDataTable->item(i>>1, i&1)->setText(data[i]);
            }
        });
        recThread->init(inEndPt, ui->saveToDisk->isChecked());
        recThread->start();
        ui->startReceiveButton->setEnabled(false);
        ui->saveToDisk->setEnabled(false);
        ui->stopReceiveButton->setEnabled(true);
    });

    // 停止接受按钮回调
    connect(ui->stopReceiveButton, &QPushButton::clicked, this, [=]()
    {
        recThread->quit();
        recThread->wait(50);
        recThread->terminate();
        delete recThread;
        ui->stopReceiveButton->setEnabled(false);
        ui->startReceiveButton->setEnabled(true);
        ui->saveToDisk->setEnabled(true);
    });

    // 数据展示初始化
    ui->originalDataTable->setRowCount(12);
    ui->originalDataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    for (int i = 0; i < 24; ++ i)
    {
        ui->originalDataTable->setItem(i>>1, i&1, new QTableWidgetItem(""));
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

