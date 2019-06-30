#-------------------------------------------------
#
# Project created by QtCreator 2018-12-01T10:41:21
#
#-------------------------------------------------

QT       += core gui widgets network xml sql

TARGET = light_controller
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

INCLUDEPATH += ../kennel_fan_controller

LIBS += -lavahi-common -lavahi-client -lavahi-qt5

SOURCES += \
    light_controller.cpp \
    light_controller_sslsock.cpp \
    i2c_ad_da_converter.cpp \
    light_state_determiner.cpp \
    main.cpp \
    zconfservice.cpp \
    zconfserviceclient.cpp \
    ../kennel_fan_controller/lin_gpio_util.cpp \
    ../kennel_fan_controller/lin_gpio.cpp \
    ../kennel_fan_controller/lin_pwm.cpp

HEADERS += \
    light_controller.h \
    light_controller_sslsock.h \
    i2c_ad_da_converter.h \
    light_state_determiner.h \
    zconfservice.h \
    zconfserviceclient.h \
    ../kennel_fan_controller/lin_gpio.h \
    ../kennel_fan_controller/lin_pwm.h \
    ../kennel_fan_controller/lin_gpio_util.h

RESOURCES += light_controller.qrc

FORMS += \
        light_controller.ui

# Default rules for deployment.
target.path = /tmp/$${TARGET}/bin
INSTALLS += target
