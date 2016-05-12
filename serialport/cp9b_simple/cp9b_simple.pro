#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T17:21:11
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cp9b_simple
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    cp9b_lib.cpp

HEADERS  += mainwindow.h \
    cp9b_lib.h

FORMS    += mainwindow.ui
