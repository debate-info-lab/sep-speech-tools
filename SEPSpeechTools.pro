#-------------------------------------------------
#
# Project created by QtCreator 2016-01-29T19:07:26
#
#-------------------------------------------------

QT       += core gui widgets webenginewidgets multimedia


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
    speechworker.cpp \
    sliderhook.cpp \
    speechoptimizer.cpp \
    wave.cpp

HEADERS  += \
    mainwindow.h \
    utility.h \
    speechcounter.h \
    mecabnode.h \
    autocursor.h \
    preferencedialog.h \
    speechsynthesizer.h \
    speechdialog.h \
    speechworker.h \
    sliderhook.h \
    speechoptimizer.h \
    wave.h

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
linux {
    include(config/linux.pri)
}
mac {
    include(config/mac.pri)
    include(config/mac_path.pri)
}
win32{
    include(config/windows.pri)
    include(config/windows_path.pri)
}
