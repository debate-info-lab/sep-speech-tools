#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QSettings>

#include <mecab.h>

#include "utility.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
#if defined(Q_OS_LINUX)
    settings(new QSettings()),
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
    settings(new QSettings(QSettings::IniFormat, QSettings::UserScope)),
#endif
    tagger(nullptr)
{
    this->ui->setupUi(this);

    this->initializeTagger(QString());
}

MainWindow::~MainWindow()
{}

void MainWindow::initializeTagger(QString dictPath, QString userDictPath)
{
    if ( dictPath.isEmpty() ) {
        QDir dir(QCoreApplication::applicationDirPath());
        dir.cd("dict");
        dictPath = this->settings->value("dict/system", dir.absolutePath()).toString();
    }
    QString taggerArgs;
    if ( userDictPath.isEmpty() ) {
        taggerArgs = QString("-d %1").arg(dictPath);
    } else {
        taggerArgs = QString("-d %1 -u %2").arg(dictPath).arg(userDictPath);
    }
    this->tagger = QSharedPointer<MeCab::Tagger>(MeCab::Tagger::create(toLocalEncoding(taggerArgs).constData()));
}

void MainWindow::setTextInformation(int charCount, int speechCount) const
{
    QString message;
    if ( speechCount < 0 ) {
        message = tr("%1 chars").arg(charCount);
    } else {
        message = tr("%1 chars, %2 syllabic").arg(charCount).arg(speechCount);
    }
    this->ui->statusBar->showMessage(message, 0);
}

void MainWindow::on_textEdit_textChanged()
{
    QString text = this->ui->textEdit->toPlainText();

    QString serialized(text);
    serialized.remove(QRegExp("\\s+"));

    int speechCount = -1;
    if ( this->tagger ) {
        speechCount = getSpeechCount(this->tagger, text);
    }

    this->setTextInformation(serialized.size(), speechCount);
}
