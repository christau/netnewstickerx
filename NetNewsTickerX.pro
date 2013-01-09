#-------------------------------------------------
#
# Project created by QtCreator 2012-11-26T11:03:34
#
#-------------------------------------------------

QT       += core gui network opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = NetNewsTickerX
TEMPLATE = app


SOURCES += main.cpp\
        tickerwindow.cpp \
    faviconmanager.cpp \
    feedmanager.cpp \
    feed.cpp \
    item.cpp \
    tickerview.cpp

HEADERS  += tickerwindow.h \
    faviconmanager.h \
    feedmanager.h \
    feed.h \
    item.h \
    tickerview.h

FORMS    += tickerwindow.ui
