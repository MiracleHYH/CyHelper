QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    loopsend.cpp \
    main.cpp \
    mainwindow.cpp \
    recthread.cpp \
    sendthread.cpp

HEADERS += \
    CyAPI.h \
    cyioctl.h \
    loopsend.h \
    mainwindow.h \
    recthread.h \
    sendthread.h \
    usb100.h \
    usb200.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/./ -lCyAPI

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/./CyAPI.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/./libCyAPI.a

RC_ICONS = usb.ico
