#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T17:21:11
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serial_stuffing
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serial_stuffing.cpp

HEADERS  += mainwindow.h \
    serial_stuffing.h

FORMS    += mainwindow.ui
