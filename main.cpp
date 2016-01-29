#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("SEP");
    QCoreApplication::setOrganizationDomain("sep.beta.jp");
    QCoreApplication::setApplicationName("SEPSpeechTools");

    MainWindow w;
    w.show();

    return a.exec();
}
