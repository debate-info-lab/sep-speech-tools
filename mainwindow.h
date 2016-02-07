#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedPointer>

namespace Ui {
class MainWindow;
}

class QSettings;

namespace MeCab {
class Tagger;
}

class PreferenceDialog;
class SpeechCounter;
class SpeechDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void initializeTagger(QString dictPath=QString(), QString userDictPath=QString());

public slots:
    // actions
    void on_actionShowRuby_triggered();
    void on_actionPreference_triggered();

    void on_textEdit_textChanged();
    void setTextInformation(int charCount, double speechCount);

private:
    QSharedPointer<Ui::MainWindow> ui;
    QSharedPointer<QSettings> settings;
    QSharedPointer<PreferenceDialog> preference;

    MeCab::Tagger *tagger;
    QSharedPointer<SpeechCounter> speechCounter;
    QSharedPointer<SpeechDialog> speechDialog;

};

#endif // MAINWINDOW_H
