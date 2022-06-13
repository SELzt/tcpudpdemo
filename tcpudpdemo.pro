QT       += core gui
QT += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    exportutil.cpp \
    frudpclient.cpp \
    frudpserver.cpp \
    main.cpp \
    tcpclient.cpp \
    tcpserver.cpp \
    widget.cpp

HEADERS += \
    constant.h \
    exportutil.h \
    frudpclient.h \
    frudpserver.h \
    head.h \
    tcpclient.h \
    tcpserver.h \
    widget.h

FORMS += \
    frudpclient.ui \
    frudpserver.ui \
    tcpclient.ui \
    tcpserver.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
