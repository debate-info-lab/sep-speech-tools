#include "speechworker.h"

#include <QDebug>

#ifndef NO_MULTIMEDIA

#include <QAudioOutput>
#include <QBuffer>

constexpr int bit_to_byte(int bit_length)
{
    return bit_length / 8;
}

template <int SampleSize>
int PosToTime(int pos)
{
    return pos / bit_to_byte(SampleSize);
}

template <int SampleSize>
int TimeToPos(int time)
{
    return time * bit_to_byte(SampleSize);
}

SpeechWorker::SpeechWorker(const QByteArray &data, const QAudioFormat &format) :
    QObject(0),
    data(data),
    output(new QAudioOutput(format)),
    buffer(new QBuffer(&this->data)),
    timer()
{
    this->buffer->open(QIODevice::ReadOnly);

    connect(&this->timer, SIGNAL(timeout()), this, SLOT(timer_timeout()));
}

SpeechWorker::~SpeechWorker()
{

}

void SpeechWorker::start()
{
    if ( this->data.isEmpty() ) {
        return;
    }
    emit this->ready();
}

QAudio::State SpeechWorker::state() const
{
    return this->output->state();
}

int SpeechWorker::size() const
{
    return PosToTime<SAMPLE_SIZE>(this->data.size());
}

void SpeechWorker::play()
{
    this->output->start(this->buffer.data());
    this->timer.start(20);
}

void SpeechWorker::pause()
{
    this->timer.stop();
    this->output->stop();
}

void SpeechWorker::stop()
{
    this->timer.stop();
    this->output->stop();
    this->buffer->seek(0);
    emit this->tick(0);
}

void SpeechWorker::seek(int time)
{
    this->buffer->seek(TimeToPos<SAMPLE_SIZE>(time));
}

void SpeechWorker::timer_timeout()
{
    emit this->tick(PosToTime<SAMPLE_SIZE>(this->buffer->pos()));
}

#endif // NO_MULTIMEDIA
