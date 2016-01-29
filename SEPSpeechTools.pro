#-------------------------------------------------
#
# Project created by QtCreator 2016-01-29T19:07:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SEPSpeechTools
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    utility.cpp

HEADERS  += \
    mainwindow.h \
    utility.h

FORMS    += mainwindow.ui

lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

LIBS += -lmecab
