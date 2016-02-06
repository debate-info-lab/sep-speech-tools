#include "wave.h"

#include <cassert>

#include <QIODevice>


class SeekContext
{
public:
    SeekContext(QIODevice *device, qint64 pos) :
        device_(device),
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

void Chunk::skip()
{
    this->device_->seek(this->pos() + this->size());
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
    format_(format),
    channels_(channels),
    framerate_(framerate),
    bitsPerSample_(bitsPerSample)
{

}

QByteArray WaveFormat::toByteArray() const
{
    return from_uint16_le(this->format_) +
            from_uint16_le(this->channels_) +
            from_uint32_le(this->framerate_) +
            from_uint32_le(this->avgBytesPerSec()) +
            from_uint16_le(this->blockAlign()) +
            from_uint16_le(this->bitsPerSample_);
}


Wave::~Wave()
{

}

const QByteArray Wave::data()
{
    SeekContext ctx(this->device_, this->pos_);
    return this->device_->read(this->size_);
}

Wave Wave::create(QIODevice *device)
{
    Chunk riff(device);
    if ( riff.header() != Header::RIFF ) {
        throw RIFFException();
    }

    uint32_t wave = to_uint32_le(device->read(4));
    if ( wave != Header::WAVE ) {
        throw RIFFWaveException();
    }

    uint16_t formatID;
    uint16_t channels;
    uint32_t framerate;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
    bool fmtReaded = false;
    while ( ! device->atEnd() ) {
        Chunk chunk(device);
        if ( chunk.header() == Header::fmt_ ) {
            formatID = to_uint16_le(device->read(2));
            if ( formatID != WaveFormat::PCM ) {
                throw UnknownFormatException();
            }
            channels = to_uint16_le(device->read(2));
            framerate = to_uint32_le(device->read(4));
            avgBytesPerSec = to_uint32_le(device->read(4));
            blockAlign = to_uint16_le(device->read(2));
            bitsPerSample = to_uint16_le(device->read(2));
            fmtReaded = true;
        } else if ( chunk.header() == Header::data ) {
            if ( ! fmtReaded ) {
                throw DataChunkException();
            }
            WaveFormat format(formatID, channels, framerate, bitsPerSample);
            if ( format.avgBytesPerSec() != avgBytesPerSec ||
                 format.blockAlign() != blockAlign ) {
                throw ParameterException();
            }
            return Wave(device, format, device->pos(), chunk.size());
        }
        chunk.skip();
    }
    throw ChunkMissingException();
}

Wave::Wave(QIODevice *device, const WaveFormat &format, qint64 pos, uint32_t size) :
    device_(device),
    format_(format),
    pos_(pos),
    size_(size)
{

}


WaveBuilder::WaveBuilder(const WaveFormat &format) :
    format(format)
{

}

QByteArray WaveBuilder::build(const QByteArray &data) const
{
    QByteArray fmt_ = this->format.toByteArray();

    QByteArray dataChunk = Chunk::makeChunk(Wave::Header::data, data.size());
    QByteArray fmt_Chunk = Chunk::makeChunk(Wave::Header::fmt_, fmt_.size());
    QByteArray wave = from_uint32_le(Wave::Header::WAVE);

    QByteArray waveHeader = wave + fmt_Chunk + fmt_ + dataChunk;
    QByteArray riffChunk = Chunk::makeChunk(Wave::Header::RIFF, waveHeader.size() + data.size());
    return riffChunk + waveHeader + data;
}


const char *RIFFException::what() const noexcept
{
    return "file does not start with RIFF id";
}

const char *RIFFWaveException::what() const noexcept
{
    return "not a WAVE file";
}

const char *UnknownFormatException::what() const noexcept
{
    return "unknown format";
}

const char *DataChunkException::what() const noexcept
{
    return "data chunk before fmt chunk";
}

const char *ParameterException::what() const noexcept
{
    return "parameter does not match";
}

const char *ChunkMissingException::what() const noexcept
{
    return "fmt chunk and/or data chunk missing";
}
