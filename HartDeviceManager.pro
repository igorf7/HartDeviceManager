#-------------------------------------------------
#
# Project created by QtCreator 2018-10-27T17:55:09
#
#-------------------------------------------------

QT       += core gui serialport charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = HartDeviceManager
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    configviewer.cpp \
    hartpro.cpp \
    moniviewer.cpp \
    port.cpp \
    device.cpp \
    mycrc.cpp \
    convert.cpp

HEADERS += \
        mainwindow.h \
    hart.h \
    hartpro.h \
    moniviewer.h \
    port.h \
    configviewer.h \
    device.h \
    mycrc.h \
    convert.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:{
    VERSION = 1.5
    QMAKE_TARGET_COMPANY = FIV
    QMAKE_TARGET_PRODUCT = HART DEVICE MANAGER
    QMAKE_TARGET_COPYRIGHT = Igor Filippov
}

RESOURCES += \
    res.qrc
win32: RC_ICONS = $$PWD/images/img.ico

