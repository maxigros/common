QT += core
QT -= gui

CONFIG += c++11

TARGET = life
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    life.cpp

HEADERS += \
    life.h
