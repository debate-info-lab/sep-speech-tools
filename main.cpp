#include "mainwindow.h"
#include <QApplication>

#include <QDir>
#include <QLocale>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("SEP");
    QCoreApplication::setOrganizationDomain("sep.beta.jp");
    QCoreApplication::setApplicationName("SEPSpeechTools");

    QDir plugins = QDir(QCoreApplication::applicationDirPath());
    if ( plugins.cd("plugins") ) {
        a.addLibraryPath(plugins.absolutePath());
    }
#ifdef Q_OS_MAC
    a.addLibraryPath(QString("%1/../Frameworks/plugins").arg(QCoreApplication::applicationDirPath()));
#endif

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
