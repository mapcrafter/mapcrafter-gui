#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T15:00:53
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mapcrafter-gui
TEMPLATE = app


SOURCES += main.cpp mainwindow.cpp \
    validationwidget.cpp inisyntaxhighlighter.cpp

HEADERS  += mainwindow.h \
    validationwidget.h inisyntaxhighlighter.h

FORMS    += mainwindow.ui

LIBS += -L ../src/mapcraftercore -l mapcraftercore 
INCLUDEPATH += ../src

win32 {
	LIBS += -l boost_system-mt -l boost_filesystem-mt
#	LIBS += -L ../lib/boost_1_54_0/stage/lib -l boost_system-mgw49-mt-1_54 -l boost_filesystem-mgw49-mt-1_54
#	INCLUDEPATH += ../lib/boost_1_54_0
}

QMAKE_CXXFLAGS += -std=c++0x
