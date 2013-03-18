#-------------------------------------------------
#
# Project created by QtCreator 2012-11-26T11:03:34
#
#-------------------------------------------------
CONFIG += debug
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
    configdialog.cpp \
    configuration.cpp \
    colorbutton.cpp

HEADERS  += tickerwindow.h \
    faviconmanager.h \
    feedmanager.h \
    feed.h \
    item.h \
    configdialog.h \
    configuration.h \
    colorbutton.h

FORMS    += tickerwindow.ui \
    configdialog.ui

mac {
    QMAKE_INFO_PLIST = NetNewsTickerX.plist
    ICON = NetNewsTickerX.icns
}
