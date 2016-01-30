#-------------------------------------------------
#
# Project created by QtCreator 2016-01-29T19:07:26
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets

TARGET = SEPSpeechTools
TEMPLATE = app

CONFIG += c++11

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    utility.cpp \
    speechcounter.cpp \
    mecabnode.cpp

HEADERS  += \
    mainwindow.h \
    utility.h \
    speechcounter.h \
    mecabnode.h

FORMS    += mainwindow.ui

lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11

LIBS += -lmecab

TRANSLATIONS = \
    SEPSpeechTools_ja.ts

RESOURCES += \
    resources.qrc
