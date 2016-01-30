#include "mainwindow.h"
#include <QApplication>

#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("SEP");
    QCoreApplication::setOrganizationDomain("sep.beta.jp");
    QCoreApplication::setApplicationName("SEPSpeechTools");

    // translations
    QTranslator qtTr;
    if ( qtTr.load("qt_" + QLocale::system().name(), ":/i18n") ) {
        a.installTranslator(&qtTr);
    }

    QTranslator appTr;
    if ( qtTr.load("SEPSpeechTools_" + QLocale::system().name(), ":/i18n") ) {
        a.installTranslator(&appTr);
    }

    MainWindow w;
    w.show();

    return a.exec();
}
