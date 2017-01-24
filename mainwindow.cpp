#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDir>
#include <QSettings>
#include <QWebEngineView>

#include <mecab.h>

#include "autocursor.h"
#include "preferencedialog.h"
#include "speechcounter.h"
#include "speechdialog.h"
#include "utility.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
#if defined(Q_OS_LINUX)
    settings(new QSettings()),
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN) || defined(Q_OS_MAC)
    settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName())),
#endif
    preference(new PreferenceDialog(this->settings)),
    tagger(nullptr),
    speechCounter(),
    speechDialog(new SpeechDialog)
{
    this->ui->setupUi(this);

    this->initializeTagger(QString());
}

MainWindow::~MainWindow()
{
    if ( this->tagger ) {
        MeCab::deleteTagger(this->tagger);
    }
}

void MainWindow::initializeTagger(QString dictPath, QString userDictPath)
{
    if ( this->tagger ) {
        this->speechDialog->close();
        this->speechCounter.reset();
        this->speechDialog->setSpeechCounter(this->speechCounter);
        MeCab::deleteTagger(this->tagger);
    }

    if ( dictPath.isEmpty() ) {
        dictPath = this->preference->getSysDictDir();
    }
    if ( userDictPath.isEmpty() ) {
        userDictPath = this->preference->getUserDictFile();
    }

    QDir appDir(QCoreApplication::applicationDirPath());
#if defined(Q_OS_MAC)
    appDir.cdUp(); // MacOS
    appDir.cdUp(); // Contents
    appDir.cdUp(); // *.app
#endif

    // build args
    QStringList taggerArgs;
    taggerArgs.append(QCoreApplication::applicationFilePath()); // argv[0]
    taggerArgs.append("-U");
    taggerArgs.append("Unknown");
    taggerArgs.append("-r");
    taggerArgs.append(appDir.filePath("mecabrc"));
    taggerArgs.append("-d");
    taggerArgs.append(dictPath + "/");
    if ( ! userDictPath.isEmpty() ) {
        taggerArgs.append("-u");
        taggerArgs.append(userDictPath);
    }
    // unicode args to raw args
    QList<QByteArray> args;
    for ( const QString &arg : taggerArgs ) {
        args.append(toLocalEncoding(arg));
    }
    // raw args to argv
    QScopedPointer<char *> argv(new char *[args.size()]);
    int argc = 0;
    for ( QByteArray &arg : args ) {
        argv.data()[argc++] = arg.data();
    }
    // create tagger
    this->tagger = MeCab::Tagger::create(argc, argv.data());
}

void MainWindow::on_actionShowRuby_triggered()
{
    if ( ! this->speechCounter ) {
        return;
    }

    this->speechDialog->setSpeechCounter(this->speechCounter);
    this->speechDialog->show();
}

void MainWindow::on_actionPreference_triggered()
{
    if ( this->preference->exec() == QDialog::Accepted ) {
        this->preference->saveSettings();
        this->initializeTagger(QString());  // from new settings
    }
}

void MainWindow::setTextInformation(int charCount, double speechCount)
{
    QString message;
    if ( speechCount < 0 ) {
        message = tr("%1 chars").arg(charCount);
    } else {
        message = tr("%1 chars, %2 syllable").arg(charCount).arg(speechCount);
    }
    this->ui->statusBar->showMessage(message, 0);
}

void MainWindow::on_textEdit_textChanged()
{
    BusyAutoCursor cursor(this);
    Q_UNUSED(cursor);

    QString text = this->ui->textEdit->toPlainText().normalized(QString::NormalizationForm_KC);

    QString serialized(text);
    serialized.remove(QRegExp("\\s+"));

    this->speechCounter = QSharedPointer<SpeechCounter>(new SpeechCounter(this->tagger, text));
    this->setTextInformation(serialized.size(), this->speechCounter->getSpeechCount());
}
