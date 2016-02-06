#include "wave.h"

#include <cassert>

#include <QIODevice>


class SeekContext
{
public:
    SeekContext(QIODevice *device, qint64 pos) :
        before_(device->pos())
    {
        this->device_->seek(pos);
    }
    ~SeekContext()
    {
        this->device_->seek(this->before_);
    }

private:
    QIODevice *device_;
    qint64 before_;

};


uint32_t to_uint16_le(const QByteArray &data)
{
    assert(data.size() == 2);
    uint16_t ret = 0;
    ret |= ((data.at(0) & 0xff) <<  0);
    ret |= ((data.at(1) & 0xff) <<  8);
    return ret;
}

QByteArray from_uint16_le(uint16_t data)
{
    QByteArray ret(2, '\0');
    ret[0] = ((data >>  0) & 0xff);
    ret[1] = ((data >>  8) & 0xff);
    return ret;
}

uint32_t to_uint32_le(const QByteArray &data)
{
    assert(data.size() == 4);
    uint32_t ret = 0;
    ret |= ((data.at(0) & 0xff) <<  0);
    ret |= ((data.at(1) & 0xff) <<  8);
    ret |= ((data.at(2) & 0xff) << 16);
    ret |= ((data.at(3) & 0xff) << 24);
    return ret;
}

QByteArray from_uint32_le(uint32_t data)
{
    QByteArray ret(4, '\0');
    ret[0] = ((data >>  0) & 0xff);
    ret[1] = ((data >>  8) & 0xff);
    ret[2] = ((data >> 16) & 0xff);
    ret[3] = ((data >> 24) & 0xff);
    return ret;
}


Chunk::Chunk(QIODevice *input) :
    device_(input),
    header_(0),
    size_(0),
    pos_(0)
{
    this->header_ = to_uint32_le(this->device_->read(4));
    this->size_ = to_uint32_le(this->device_->read(4));
    this->pos_ = this->device_->pos();
}

Chunk::~Chunk()
{

}

QByteArray Chunk::data()
{
    SeekContext ctx(this->device_, this->pos_);
    return this->device_->read(this->size());
}

QByteArray Chunk::makeChunk(uint32_t header, const uint32_t &size)
{
    return from_uint32_le(header) + from_uint32_le(size);
}

QByteArray Chunk::makeChunk(uint32_t header, const QByteArray &data)
{
    return Chunk::makeChunk(header, data.size()) + data;
}


WaveFormat::WaveFormat(uint16_t format, uint16_t channels, uint32_t framerate, uint16_t bitsPerSample) :
    format(format),
    channels(channels),
    framerate(framerate),
    bitsPerSample(bitsPerSample)
{

}

QByteArray WaveFormat::toByteArray() const
{
    return from_uint16_le(this->format) +
            from_uint16_le(this->channels) +
            from_uint32_le(this->framerate) +
            from_uint32_le(this->avgBytesPerSec()) +
            from_uint16_le(this->blockAlign()) +
            from_uint16_le(this->bitsPerSample);
}



Wave::Wave(const WaveFormat &format) :
    format(format)
{

}

QByteArray Wave::build(const QByteArray &data) const
{
    QByteArray fmt_ = this->format.toByteArray();

    QByteArray dataChunk = Chunk::makeChunk(Header::data, data.size());
    QByteArray fmt_Chunk = Chunk::makeChunk(Header::fmt_, fmt_.size());
    QByteArray wave = from_uint32_le(Header::WAVE);

    QByteArray waveHeader = wave + fmt_Chunk + fmt_ + dataChunk;
    QByteArray riffChunk = Chunk::makeChunk(Header::RIFF, waveHeader.size() + data.size());
    return riffChunk + waveHeader + data;
}
