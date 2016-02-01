#include "speechdialog.h"
#include "ui_speechdialog.h"

#if USE_MULTIMEDIA
#include <QAudio>
#include <QAudioDecoder>
#include <QAudioFormat>
#include <QAudioOutput>
#include <QBuffer>
#endif

#include <QFileDialog>
#include <QMessageBox>

#include "autocursor.h"
#include "speechcounter.h"
#include "speechsynthesizer.h"
#include "speechworker.h"

SpeechDialog::SpeechDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SpeechDialog),
#if USE_MULTIMEDIA
    format(),
    output(),
#endif
    audioThread(),
    speechWorker(nullptr),
    speechSynthesizer(new SpeechSynthesizer)
{
    ui->setupUi(this);

    // TODO: multimedia...
    this->ui->toolButtonPlay->setVisible(false);
    this->ui->toolButtonStop->setVisible(false);

#if USE_MULTIMEDIA
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

#if USE_MULTIMEDIA
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
#else
    /*
    QString speech = this->speechCounter->toSpeech();
    if ( speech.isEmpty() ) {
        return;
    }

    QByteArray wave = this->speechSynthesizer->synthesize(speech);
    if ( wave.isEmpty() ) {
        return;
    }

    this->speechWorker = QSharedPointer<SpeechWorker>(new SpeechWorker(wave));
    this->speechWorker->moveToThread(&this->audioThread);
    connect(this->ui->toolButtonPlay, SIGNAL(clicked()), this->speechWorker.data(), SLOT(play()));
    connect(this->ui->toolButtonStop, SIGNAL(clicked()), this->speechWorker.data(), SLOT(stop()));
    */
#endif
}

void SpeechDialog::on_pushButtonSave_clicked()
{
    BusyAutoCursor cursor(this);
    Q_UNUSED(cursor);

    QString speech = this->speechCounter->toSpeech();
    if ( speech.isEmpty() ) {
        return;
    }

    QByteArray wave = this->speechSynthesizer->synthesize(speech);
    if ( wave.isEmpty() ) {
        QMessageBox::information(this, tr("Failed"), tr("Failed to synthesize."));
        return;
    }

    QString filePath = QFileDialog::getSaveFileName(this, tr("Save file name"), QString(), tr("Wave (*.wav)"));
    if ( filePath.isEmpty() ) {
        return;
    }
#if defined(Q_OS_UNIX)
    QFileInfo info(filePath);
    if ( info.suffix() != "wav" ) {
        filePath += ".wav";
    }
#endif

    QFile file(filePath);
    if ( ! file.open(QIODevice::WriteOnly) ) {
        return;
    }
    file.write(wave);
    file.close();
}

void SpeechDialog::on_toolButtonPlay_clicked()
{
#if USE_MULTIMEDIA
    if ( this->speechWorker->state() != QAudio::ActiveState ) {
        QMetaObject::invokeMethod(this->speechWorker.data(), "play");
    } else {
        QMetaObject::invokeMethod(this->speechWorker.data(), "resume");
    }
#endif
}

void SpeechDialog::on_toolButtonStop_clicked()
{
#if USE_MULTIMEDIA
    QMetaObject::invokeMethod(this->speechWorker.data(), "stop");
#endif
}

void SpeechDialog::audioHasReady()
{
    this->speechWorker->moveToThread(&this->audioThread);
    this->ui->toolButtonPlay->setEnabled(true);
}
