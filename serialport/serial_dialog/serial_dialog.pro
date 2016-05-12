#-------------------------------------------------
#
# Project created by QtCreator 2016-04-24T17:21:11
#
#-------------------------------------------------

QT       += core gui network serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = serial_dialog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    serial_stuffing.cpp \
    serial_prefix.cpp

HEADERS  += mainwindow.h \
    serial_stuffing.h \
    serial_prefix.h \
    vars.h

FORMS    += mainwindow.ui
