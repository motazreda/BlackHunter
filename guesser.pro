#-------------------------------------------------
#
# Project created by QtCreator 2016-09-02T13:09:33
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = guesser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sniffing.cpp \
    startthread.cpp \
    receiver.cpp

HEADERS  += mainwindow.h \
    sniffing.h \
    startthread.h \
    receiver.h

FORMS    += mainwindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/release/ -ltins
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../usr/local/lib/debug/ -ltins
else:unix: LIBS += -L$$PWD/../../../usr/local/lib/ -ltins

INCLUDEPATH += $$PWD/../../../usr/local/include
DEPENDPATH += $$PWD/../../../usr/local/include
