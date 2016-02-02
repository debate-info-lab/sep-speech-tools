#include "speechworker.h"

#ifndef NO_MULTIMEDIA

#include <QAudioOutput>
#include <QBuffer>

SpeechWorker::SpeechWorker(const QByteArray &data, const QAudioFormat &format) :
    QObject(0),
    output(new QAudioOutput(format)),
    buffer(new QBuffer(&this->data)),
    data(data)
{
    this->buffer->open(QIODevice::ReadOnly);
}

SpeechWorker::~SpeechWorker()
{

}

QAudio::State SpeechWorker::state() const
{
    return this->output->state();
}

void SpeechWorker::play()
{
    this->output->start(this->buffer.data());
}

void SpeechWorker::pause()
{
    this->output->stop();
}

void SpeechWorker::stop()
{
    this->output->stop();
    this->buffer->seek(0);
}

#endif // NO_MULTIMEDIA
