#include "speechdialog.h"
#include "ui_speechdialog.h"

#include <QAudio>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>

#include "speechcounter.h"
#include "speechsynthesizer.h"
#include "speechworker.h"

SpeechDialog::SpeechDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpeechDialog),
#if QT_VERSION >= 0x050000
    audioThread(),
    speechWorker(nullptr),
    format(),
    output(),
#endif
    speechSynthesizer(new SpeechSynthesizer)
{
    ui->setupUi(this);

#if QT_VERSION >= 0x050000
    // AquesTalk output format
    this->format.setChannelCount(1);
    this->format.setCodec("audio/x-raw");
    this->format.setSampleType(QAudioFormat::UnSignedInt);
    this->format.setSampleRate(8000);
    this->format.setSampleSize(16);

    this->output = QSharedPointer<QAudioOutput>(new QAudioOutput(this->format));
#endif
}

SpeechDialog::~SpeechDialog()
{

}

void SpeechDialog::setSpeechCounter(const QSharedPointer<SpeechCounter> &counter)
{
    // TODO: audio stop

    this->speechCounter = counter;

    if ( ! this->speechCounter ) {
        this->ui->webView->setHtml("");
        return;
    }
    this->ui->webView->setHtml(this->speechCounter->toRubyHtml());

#if QT_VERSION >= 0x050000
    this->ui->toolButtonPlay->setEnabled(false);
    this->ui->toolButtonStop->setEnabled(false);

    QString speech = this->speechCounter->toSpeech();
    if ( speech.isEmpty() ) {
        return;
    }

    QByteArray wave = this->speechSynthesizer->synthesize(speech);
    if ( wave.isEmpty() ) {
        return;
    }

    this->speechWorker = QSharedPointer<SpeechWorker>(new SpeechWorker(wave, this->format));
    connect(this->speechWorker.data(), SIGNAL(ready()), this, SLOT(audioHasReady()));
    this->speechWorker->start();
#endif
}

void SpeechDialog::on_toolButtonPlay_clicked()
{
    if ( this->speechWorker->state() != QAudio::ActiveState ) {
        QMetaObject::invokeMethod(this->speechWorker.data(), "play");
    } else {
        QMetaObject::invokeMethod(this->speechWorker.data(), "resume");
    }
}

void SpeechDialog::on_toolButtonStop_clicked()
{
    QMetaObject::invokeMethod(this->speechWorker.data(), "stop");
}

void SpeechDialog::audioHasReady()
{
    this->speechWorker->moveToThread(&this->audioThread);
    this->ui->toolButtonPlay->setEnabled(true);
}
