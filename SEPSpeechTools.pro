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
    mecabnode.cpp \
    autocursor.cpp \
    preferencedialog.cpp

HEADERS  += \
    mainwindow.h \
    utility.h \
    speechcounter.h \
    mecabnode.h \
    autocursor.h \
    preferencedialog.h

FORMS    += \
    mainwindow.ui \
    preferencedialog.ui

TRANSLATIONS = \
    SEPSpeechTools_ja.ts

RESOURCES += \
    resources.qrc

unix{
    lessThan(QT_MAJOR_VERSION, 5): QMAKE_CXXFLAGS += -std=c++11
    LIBS += -lmecab
}
win32{
    include(config/windows.pri)
}
