#include "speechworker.h"

#include <QAudioDecoder>
#include <QAudioOutput>
#include <QBuffer>

SpeechWorker::SpeechWorker(const QByteArray &data, const QAudioFormat &format) :
    QObject(0),
    data(data),
    decoder(new QAudioDecoder(this)),
    output(new QAudioOutput(format)),
    buffer(new QBuffer(&this->data)),
    outputDevice(nullptr)
{
    this->decoder->setAudioFormat(format);
    this->decoder->setSourceDevice(this->buffer.data());
    connect(this->decoder.data(), SIGNAL(bufferReady()), this, SIGNAL(ready()));
}

SpeechWorker::~SpeechWorker()
{

}

QAudio::State SpeechWorker::state() const
{
    return this->output->state();
}

void SpeechWorker::start()
{
    this->decoder->start();
}

void SpeechWorker::play()
{
    this->outputDevice = this->output->start();

    QAudioBuffer buffer = this->decoder->read();
    this->outputDevice->write(buffer.constData<char>(), buffer.byteCount());
}

void SpeechWorker::resume()
{
    this->output->resume();
}

void SpeechWorker::stop()
{
    this->output->stop();
}

