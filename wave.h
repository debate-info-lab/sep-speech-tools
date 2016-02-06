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

    uint16_t blockAlign() const
    {
        return this->channels * (this->bitsPerSample >> 3);
    }
    uint32_t avgBytesPerSec() const
    {
        return this->framerate * this->blockAlign();
    }

    enum Format : uint16_t
    {
        PCM = 0x0001,
    };

private:
    uint16_t format;
    uint16_t channels;
    uint32_t framerate;
    uint16_t bitsPerSample;

};

class Wave
{
public:
    Wave(const WaveFormat &format);

    QByteArray build(const QByteArray &data) const;

    struct Header
    {
        //static const uint32_t RIFF = 0x52494646;  // "RIFF"
        //static const uint32_t WAVE = 0x57415645;  // "WAVE"
        //static const uint32_t fmt_ = 0x666d7420;  // "fmt "
        //static const uint32_t data = 0x64617461;  // "data"
        static const uint32_t RIFF = 0x46464952;  // "RIFF"
        static const uint32_t WAVE = 0x45564157;  // "WAVE"
        static const uint32_t fmt_ = 0x20746d66;  // "fmt "
        static const uint32_t data = 0x61746164;  // "data"
    };

private:
    WaveFormat format;

};

#endif // SEPSPEECHTOOLS_WAVE_H
