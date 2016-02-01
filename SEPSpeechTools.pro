#-------------------------------------------------
#
# Project created by QtCreator 2016-01-29T19:07:26
#
#-------------------------------------------------

QT       += core gui webkit

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webkitwidgets multimedia

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
    preferencedialog.cpp \
    speechsynthesizer.cpp \
    speechdialog.cpp \
    speechworker.cpp

HEADERS  += \
    mainwindow.h \
    utility.h \
    speechcounter.h \
    mecabnode.h \
    autocursor.h \
    preferencedialog.h \
    speechsynthesizer.h \
    speechdialog.h \
    speechworker.h

FORMS    += \
    mainwindow.ui \
    preferencedialog.ui \
    speechdialog.ui

TRANSLATIONS = \
    SEPSpeechTools_ja.ts

RESOURCES += \
    resources.qrc

unix{
    include(config/unix.pri)
    include(config/unix_path.pri)
}
win32{
    include(config/windows.pri)
    include(config/windows_path.pri)
}
