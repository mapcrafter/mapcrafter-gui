#-------------------------------------------------
#
# Project created by QtCreator 2014-05-12T15:00:53
#
#-------------------------------------------------

QT       += core gui
CONFIG += qt debug

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = mapcrafter-gui
TEMPLATE = app

SOURCES += main.cpp mainwindow.cpp \
    validationwidget.cpp inisyntaxhighlighter.cpp \
    logwidget.cpp \
    mapselectionwidget.cpp \
    renderworker.cpp

HEADERS  += mainwindow.h \
    validationwidget.h inisyntaxhighlighter.h \
    logwidget.h \
    mapselectionwidget.h \
    renderworker.h

FORMS    += mainwindow.ui

MAPCRAFTER_DIR = $$(MAPCRAFTER_DIR)
equals(MAPCRAFTER_DIR, "") {
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
