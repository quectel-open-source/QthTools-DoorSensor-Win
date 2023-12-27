QT       += core gui
QT       += serialport #串口
QT       += sql
QT       += printsupport  #打印机
QT       += xlsx  #表格


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#include ($$PWD/third/QSimpleUpdater/QSimpleUpdater.pri)
CONFIG += c++11

QMAKE_CFLAGS_RELEASE += -g
QMAKE_CXXFLAGS_RELEASE += -g
QMAKE_CFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_LFLAGS_RELEASE = -mthreads -W


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += HAVE_CONFIG_H
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



SOURCES += \
    app/appConfig.cpp \
    app/appinfosave.cpp \
    app/communiteconfigure.cpp \
    app/dialogoperateerror.cpp \
    app/dialogsetresult.cpp \
    app/dialogtipscombinedend.cpp \
    app/dialogtotalresult.cpp \
    app/saverecord.cpp \
    app/userinfo.cpp \
    externWidget/myprinterdialog.cpp \
    externWidget/myswitchbutton.cpp \
    externWidget/mytoolbox.cpp \
    kernel/jsonFile.cpp \
    kernel/kernelconfig.cpp \
    kernel/mysqlite.cpp \
    kernel/serial.cpp \
    kernel/serialdatahandle.cpp \
    kernel/usebarcode.cpp \
    kernel/useprinter.cpp \
    main.cpp \
    mainattribute.cpp \
    mainwindow.cpp \
    third/barcode.cpp \
    third/ccrashstack.cpp \
    third/qrencode/bitstream.c \
    third/qrencode/mask.c \
    third/qrencode/mmask.c \
    third/qrencode/mqrspec.c \
    third/qrencode/qrencode.c \
    third/qrencode/qrinput.c \
    third/qrencode/qrspec.c \
    third/qrencode/rsecc.c \
    third/qrencode/split.c

HEADERS += \
    app/appConfig.h \
    app/appinfosave.h \
    app/communiteconfigure.h \
    app/dialogoperateerror.h \
    app/dialogsetresult.h \
    app/dialogtipscombinedend.h \
    app/dialogtotalresult.h \
    app/saverecord.h \
    app/userinfo.h \
    externWidget/myprinterdialog.h \
    externWidget/myswitchbutton.h \
    externWidget/mytoolbox.h \
    kernel/jsonFile.h \
    kernel/kernelconfig.h \
    kernel/mysqlite.h \
    kernel/serial.h \
    kernel/serialdatahandle.h \
    kernel/usebarcode.h \
    kernel/useprinter.h \
    mainattribute.h \
    mainwindow.h \
    third/barcode.h \
    third/ccrashstack.h \
    third/qrencode/bitstream.h \
    third/qrencode/config.h \
    third/qrencode/mask.h \
    third/qrencode/mmask.h \
    third/qrencode/mqrspec.h \
    third/qrencode/qrencode.h \
    third/qrencode/qrencode_inner.h \
    third/qrencode/qrinput.h \
    third/qrencode/qrspec.h \
    third/qrencode/rsecc.h \
    third/qrencode/split.h \
    userConfig.h

FORMS += \
    app/communiteconfigure.ui \
    app/dialogoperateerror.ui \
    app/dialogsetresult.ui \
    app/dialogtipscombinedend.ui \
    app/dialogtotalresult.ui \
    app/saverecord.ui \
    app/userinfo.ui \
    externWidget/myprinterdialog.ui \
    mainwindow.ui \
    third/QSimpleUpdater/src/Downloader.ui \
    third/QSimpleUpdater/toolupdate.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    source.qrc \

RC_FILE += image/quectel.rc

DISTFILES += \
    third/QSimpleUpdater/etc/icon.png \
    third/QSimpleUpdater/etc/resources/restart.bat \
    third/QSimpleUpdater/etc/resources/update.png \
    third/QSimpleUpdater/etc/screenshots/download-complete.png \
    third/QSimpleUpdater/etc/screenshots/downloading.png \
    third/QSimpleUpdater/etc/screenshots/tutorial.png
