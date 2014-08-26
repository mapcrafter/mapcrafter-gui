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

!defined(MAPCRAFTER_DIR, var) {
	MAPCRAFTER_DIR = ../mapcrafter
}

LIBS += -L $${MAPCRAFTER_DIR}/src/mapcraftercore -lmapcraftercore 
INCLUDEPATH += $${MAPCRAFTER_DIR}/src

win32 {
	LIBS += -lboost_system-mt -lboost_filesystem-mt
} else {
	LIBS += -lboost_system -lboost_filesystem
}

QMAKE_CXXFLAGS += -std=c++0x
