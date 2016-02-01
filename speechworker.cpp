#include "speechworker.h"

#if USE_MULTIMEDIA
#include <QAudioDecoder>
#include <QAudioOutput>
#include <QBuffer>
#else
#include <QMediaPlayer>
#endif

#if USE_MULTIMEDIA
SpeechWorker::SpeechWorker(const QByteArray &data, const QAudioFormat &format) :
    QObject(0),
    decoder(new QAudioDecoder(this)),
    output(new QAudioOutput(format)),
    buffer(new QBuffer(&this->data)),
    outputDevice(nullptr),
    data(data)
#else
SpeechWorker::SpeechWorker(const QByteArray &data) :
    QObject(0),
    player(new QMediaPlayer),
    temp(),
    data(data)
#endif
{
#if USE_MULTIMEDIA
    this->decoder->setAudioFormat(format);
    this->decoder->setSourceDevice(this->buffer.data());
    connect(this->decoder.data(), SIGNAL(bufferReady()), this, SIGNAL(ready()));
#else
    this->temp.setFileTemplate(this->temp.fileTemplate()+".wav");
    if ( this->temp.open() ) {
        this->temp.write(data);
        this->temp.close();
    }
    this->temp.setAutoRemove(true);
    qDebug() << this->temp.fileName();
    this->player->setMedia(QUrl::fromLocalFile(this->temp.fileName()));
#endif
}

SpeechWorker::~SpeechWorker()
{

}

#if USE_MULTIMEDIA
QAudio::State SpeechWorker::state() const
{
    return this->output->state();
}

void SpeechWorker::start()
{
    this->decoder->start();
}
#endif

void SpeechWorker::play()
{
#if USE_MULTIMEDIA
    this->outputDevice = this->output->start();

    QAudioBuffer buffer = this->decoder->read();
    this->outputDevice->write(buffer.constData<char>(), buffer.byteCount());
#else
    this->player->play();
#endif
}

void SpeechWorker::resume()
{
#if USE_MULTIMEDIA
    this->output->resume();
#endif
}

void SpeechWorker::stop()
{
#if USE_MULTIMEDIA
    this->output->stop();
#else
    this->player->stop();
#endif
}

