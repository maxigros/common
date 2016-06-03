#-------------------------------------------------
#
# Project created by QtCreator 2014-04-22T23:26:15
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = skirl_painter
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp\
        viewwindow.cpp\
        qfieldview.cpp\
        viewwindow_settings.cpp

HEADERS  += mainwindow.h\
            viewwindow.h\
            qfieldview.h\
            colorpalette.h

FORMS    += mainwindow.ui\
            viewwindow.ui\
            viewwindow_help.ui

RESOURCES += skirl_fe_pixmaps.qrc

INCLUDEPATH += "c:/qwt/include"
INCLUDEPATH += "/usr/include/qwt"

LIBS += -lqwt -Lc:/qwt/lib
