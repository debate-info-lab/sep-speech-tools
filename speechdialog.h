#ifndef SPEECHDIALOG_H
#define SPEECHDIALOG_H

#include <QBuffer>
#include <QDialog>
#include <QSharedPointer>
#include <QThread>

#ifndef NO_MULTIMEDIA
#include <QAudioFormat>

class QAudioDecoder;
class QAudioOutput;
#endif

namespace Ui {
class SpeechDialog;
}

class SpeechCounter;
class SpeechSynthesizer;
class SpeechWorker;

class SpeechDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeechDialog(QWidget *parent = 0);
    ~SpeechDialog();

    void setSpeechCounter(const QSharedPointer<SpeechCounter> &counter);

public slots:
    void on_pushButtonSave_clicked();
    void on_toolButtonPlay_clicked();
    void on_toolButtonStop_clicked();

protected:
    void closeEvent(QCloseEvent *);

private:
    void waveGenerate();

    QSharedPointer<Ui::SpeechDialog> ui;
#ifndef NO_MULTIMEDIA
    QAudioFormat format;
    QSharedPointer<SpeechWorker> speechWorker;
#endif

    QSharedPointer<SpeechCounter> speechCounter;
    QSharedPointer<SpeechSynthesizer> speechSynthesizer;
    bool waveGenerated;
    QByteArray waveData;

};

#endif // SPEECHDIALOG_H
