#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T15:00:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mapcrafter-gui
TEMPLATE = app


SOURCES += main.cpp mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

LIBS += -L ../mapcrafter/src/mapcraftercore -l mapcraftercore
INCLUDEPATH += ../mapcrafter/src

QMAKE_CXXFLAGS += -std=c++11
