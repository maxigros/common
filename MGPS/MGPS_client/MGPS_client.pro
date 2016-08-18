#-------------------------------------------------
#
# Project created by QtCreator 2016-08-17T11:16:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MGPS_client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    llmdb_client.cpp \
    llmdb_client_ext.cpp

HEADERS  += mainwindow.h \
    llmdb_client.h \
    defines.h \
    llmdb_client_ext.h

FORMS    += mainwindow.ui
