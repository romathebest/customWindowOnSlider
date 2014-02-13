#-------------------------------------------------
#
# Project created by QtCreator 2014-02-12T12:07:52
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TestWindowEffect
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    sliderwidget.cpp \
    customwindow.cpp

HEADERS  += mainwindow.h \
    sliderwidget.h \
    customwindow.h

FORMS    += mainwindow.ui \
    customwindow.ui

RESOURCES += \
    resources.qrc
