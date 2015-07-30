#-------------------------------------------------
#
# Project created by QtCreator 2015-07-19T01:50:54
#
#-------------------------------------------------

QT       += core gui widgets multimedia network testlib

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = player
TEMPLATE = app


SOURCES += main.cpp \
    mainwindow.cpp \
    playlist.cpp

HEADERS  += \
    mainwindow.h \
    playlist.h

FORMS    += \
    mainwindow.ui \
    playlist.ui

RESOURCES += \
    resources.qrc

OTHER_FILES +=
