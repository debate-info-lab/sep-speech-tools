#ifndef SEPSPEECHTOOLS_WAVE_H
#define SEPSPEECHTOOLS_WAVE_H

#include <cinttypes>
#include <qglobal.h>

class QIODevice;

class Chunk
{
public:
    Chunk(QIODevice *input);
    ~Chunk();

    uint32_t header() const
    { return this->header_; }
    uint32_t size() const
    { return this->size_; }
    qint64 pos() const
    { return this->pos_; }

    void skip();
    QByteArray data();

    static QByteArray makeChunk(uint32_t header, const uint32_t &size);
    static QByteArray makeChunk(uint32_t header, const QByteArray &data);

private:
    QIODevice *device_;
    uint32_t header_;
    uint32_t size_;
    qint64 pos_;

};

class WaveFormat
{
public:
    WaveFormat(uint16_t format, uint16_t channels, uint32_t framerate, uint16_t bitsPerSample);

    QByteArray toByteArray() const;

    uint16_t format() const
    { return this->format_; }
    uint16_t channels() const
    { return this->channels_; }
    uint32_t framerate() const
    { return this->framerate_; }
    uint16_t bitsPerSample() const
    { return this->bitsPerSample_; }

    uint16_t blockAlign() const
    {
        return this->channels() * (this->bitsPerSample() >> 3);
    }
    uint32_t avgBytesPerSec() const
    {
        return this->framerate() * this->blockAlign();
    }

    enum Format : uint16_t
    {
        PCM = 0x0001,
    };

private:
    uint16_t format_;
    uint16_t channels_;
    uint32_t framerate_;
    uint16_t bitsPerSample_;

};


class Wave
{
public:
    ~Wave();

    const WaveFormat format() const
    { return format_; }

    const QByteArray data();

    struct Header
    {
        static const uint32_t RIFF = 0x46464952;  // "RIFF"
        static const uint32_t WAVE = 0x45564157;  // "WAVE"
        static const uint32_t fmt_ = 0x20746d66;  // "fmt "
        static const uint32_t data = 0x61746164;  // "data"
    };

    static Wave create(QIODevice *device);

private:
    Wave(QIODevice *device, const WaveFormat &format, qint64 pos, uint32_t size);

    QIODevice *device_;
    WaveFormat format_;
    qint64 pos_;
    uint32_t size_;

};


class WaveBuilder
{
public:
    WaveBuilder(const WaveFormat &format);

    QByteArray build(const QByteArray &data) const;

private:
    WaveFormat format;

};


class WaveException : public std::exception
{};

class RIFFException : public WaveException
{
public:
    ~RIFFException()
    {}
    const char *what() const noexcept;
};

class RIFFWaveException : public WaveException
{
public:
    ~RIFFWaveException()
    {}
    const char *what() const noexcept;
};

class UnknownFormatException : public WaveException
{
public:
    ~UnknownFormatException()
    {}
    const char *what() const noexcept;
};

class DataChunkException : public WaveException
{
public:
    ~DataChunkException()
    {}
    const char *what() const noexcept;
};

class ParameterException : public WaveException
{
public:
    ~ParameterException()
    {}
    const char *what() const noexcept;
};

class ChunkMissingException : public WaveException
{
public:
    ~ChunkMissingException()
    {}
    const char *what() const noexcept;
};

#endif // SEPSPEECHTOOLS_WAVE_H
