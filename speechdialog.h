#ifndef SPEECHDIALOG_H
#define SPEECHDIALOG_H

#include <QBuffer>
#include <QDialog>
#include <QSharedPointer>
#include <QThread>

#if USE_MULTIMEDIA
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

private slots:
    void audioHasReady();

private:
    QSharedPointer<Ui::SpeechDialog> ui;
#if USE_MULTIMEDIA
    QAudioFormat format;
    QSharedPointer<QAudioOutput> output;
#endif
    QThread audioThread;
    QSharedPointer<SpeechWorker> speechWorker;

    QSharedPointer<SpeechCounter> speechCounter;
    QSharedPointer<SpeechSynthesizer> speechSynthesizer;

};

#endif // SPEECHDIALOG_H
