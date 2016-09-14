#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T11:16:25
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MGPS_client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cp9b_client.cpp \
    cp9b_client_ext.cpp

HEADERS  += mainwindow.h \
    cp9b_client.h \
    defines.h \
    cp9b_client_ext.h

FORMS    += mainwindow.ui
