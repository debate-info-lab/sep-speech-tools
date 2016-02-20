#include "speechdialog.h"
#include "ui_speechdialog.h"

#ifndef NO_MULTIMEDIA
#include <QAudio>

#include "sliderhook.h"
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
#ifndef NO_MULTIMEDIA
    format(),
    speechWorker(nullptr),
    sliderHook(nullptr),
#endif
    speechSynthesizer(new SpeechSynthesizer),
    waveGenerated(false),
    waveData()
{
    ui->setupUi(this);

#ifndef NO_MULTIMEDIA
    // AquesTalk output format
    this->format.setChannelCount(1);
    this->format.setCodec("audio/pcm");
    this->format.setByteOrder(QAudioFormat::LittleEndian);
    this->format.setSampleType(QAudioFormat::SignedInt);
    this->format.setSampleRate(8000);
    this->format.setSampleSize(16);

    this->sliderHook = QSharedPointer<SliderHook>(new SliderHook(this->ui->horizontalSlider));
    this->ui->horizontalSlider->installEventFilter(this->sliderHook.data());
#else
    this->ui->toolButtonPlay->setVisible(false);
    this->ui->toolButtonStop->setVisible(false);
    this->ui->horizontalSlider->setVisible(false);
#endif
}

SpeechDialog::~SpeechDialog()
{

}

void SpeechDialog::setSpeechCounter(const QSharedPointer<SpeechCounter> &counter)
{
#ifndef NO_MULTIMEDIA
    if ( this->speechWorker ) {
        this->speechWorker->stop();
    }
#endif

    this->speechCounter = counter;
    this->waveGenerated = false;
    this->ui->horizontalSlider->setEnabled(false);

    if ( ! this->speechCounter ) {
        this->ui->webView->setHtml("");
        return;
    }
    this->ui->webView->setHtml(this->speechCounter->toRubyHtml());
}

void SpeechDialog::on_pushButtonSave_clicked()
{
    this->waveGenerate();

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
        QMessageBox::warning(this, tr("Failed"), tr("Failed to write."));
        return;
    }
    file.write(this->waveData);
    file.close();
}

void SpeechDialog::on_toolButtonPlay_clicked()
{
#ifndef NO_MULTIMEDIA
    this->waveGenerate();

    if ( ! this->speechWorker ) {
        return;
    }

    if ( this->speechWorker->state() != QAudio::ActiveState ) {
        QMetaObject::invokeMethod(this->speechWorker.data(), "play");
    } else {
        QMetaObject::invokeMethod(this->speechWorker.data(), "pause");
    }
#endif
}

void SpeechDialog::on_toolButtonStop_clicked()
{
#ifndef NO_MULTIMEDIA
    if ( ! this->speechWorker ) {
        return;
    }

    QMetaObject::invokeMethod(this->speechWorker.data(), "stop");
#endif
}

void SpeechDialog::audioHasReady()
{
#ifndef NO_MULTIMEDIA
    this->ui->horizontalSlider->setEnabled(true);
    this->ui->horizontalSlider->setMaximum(this->speechWorker->size());
#endif
}

void SpeechDialog::closeEvent(QCloseEvent *)
{
#ifndef NO_MULTIMEDIA
    if ( this->speechWorker ) {
        this->speechWorker->stop();
    }
#endif
}

void SpeechDialog::waveGenerate()
{
    if ( this->waveGenerated ) {
        return;
    }
    BusyAutoCursor cursor(this);
    Q_UNUSED(cursor);

#ifndef NO_AQUESTALK
    QString speech = this->speechCounter->toSpeechForAquesTalk();
#else
    QString speech = this->speechCounter->toSpeech();
#endif
    if ( speech.isEmpty() ) {
        return;
    }

    QByteArray wave;
    try {
        wave = this->speechSynthesizer->synthesize(speech);
#ifndef NO_AQUESTALK
    } catch (const TooLongInputException &) {
        QMessageBox::critical(this, tr("Error"), tr("Sentence is too long."));
        return;
#endif // NO_AQUESTALK
    } catch (...) {
        throw;
    }
    if ( wave.isEmpty() ) {
        return;
    }
    this->waveData = wave;
    this->waveGenerated = true;

#ifndef NO_MULTIMEDIA
    if ( this->speechWorker ) {
        disconnect(this->speechWorker.data(), SIGNAL(ready()), this, SLOT(audioHasReady()));
        disconnect(this->speechWorker.data(), SIGNAL(tick(int)), this->ui->horizontalSlider, SLOT(setValue(int)));
        disconnect(this->ui->horizontalSlider, SIGNAL(valueChanged(int)), this->speechWorker.data(), SLOT(seek(int)));
    }
    this->speechWorker = QSharedPointer<SpeechWorker>(new SpeechWorker(this->waveData.mid(44), this->format));

    connect(this->speechWorker.data(), SIGNAL(ready()), this, SLOT(audioHasReady()));
    connect(this->speechWorker.data(), SIGNAL(tick(int)), this->ui->horizontalSlider, SLOT(setValue(int)));
    connect(this->ui->horizontalSlider, SIGNAL(valueChanged(int)), this->speechWorker.data(), SLOT(seek(int)));
    this->speechWorker->start();

#endif
}
