#-------------------------------------------------
#
# Project created by QtCreator 2014-06-02T09:49:36
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = light_controller_remote_ui
TEMPLATE = app

SOURCES += main.cpp\
        light_controller_remote_ui.cpp \
        ssl_client.cpp \
        qt-watch.cpp \
        zconfservice.cpp \
        zconfserviceclient.cpp \
        zconfservicebrowser.cpp

HEADERS  += \
        light_controller_remote_ui.h \
        ssl_client.h \
        qt-watch.h \
        zconfservice.h \
        zconfserviceclient.h \
        zconfservicebrowser.h

RESOURCES += light_controller_remote_ui.qrc

FORMS    += light_controller_remote_ui.ui

LIBS += -lavahi-client -lavahi-common

CONFIG += mobility
MOBILITY = 

# install
target.path = /afs/aaker.org/home/kdaaker/src/light_controller/ligh_controller_remote_ui
INSTALLS += target

