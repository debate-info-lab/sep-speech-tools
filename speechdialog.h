#ifndef SPEECHDIALOG_H
#define SPEECHDIALOG_H

#include <QBuffer>
#include <QDialog>
#include <QSharedPointer>

#if QT_VERSION >= 0x050000
#include <QAudioFormat>
#include <QThread>

class QAudioDecoder;
class QAudioOutput;

class SpeechWorker;
#endif

namespace Ui {
class SpeechDialog;
}

class SpeechCounter;
class SpeechSynthesizer;

class SpeechDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SpeechDialog(QWidget *parent = 0);
    ~SpeechDialog();

    void setSpeechCounter(const QSharedPointer<SpeechCounter> &counter);

public slots:
    void on_toolButtonPlay_clicked();
    void on_toolButtonStop_clicked();

private slots:
    void audioHasReady();

private:
    QSharedPointer<Ui::SpeechDialog> ui;
    QByteArray audioData;
    QSharedPointer<QBuffer> audioBuffer;
#if QT_VERSION >= 0x050000
    QThread audioThread;
    QSharedPointer<SpeechWorker> speechWorker;
    QAudioFormat format;
    QSharedPointer<QAudioOutput> output;
#endif

    QSharedPointer<SpeechCounter> speechCounter;
    QSharedPointer<SpeechSynthesizer> speechSynthesizer;

};

#endif // SPEECHDIALOG_H
