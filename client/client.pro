#-------------------------------------------------
#
# Project created by QtCreator 2019-09-06T21:04:20
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = client
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
QMAKE_CXXFLAGS += -std=c++11
