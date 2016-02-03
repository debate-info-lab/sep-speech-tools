#ifndef SPEECHSYNTHESIZER_H
#define SPEECHSYNTHESIZER_H

#include <exception>

#include <QByteArray>
#include <QSharedPointer>


class SpeechSynthesizerImpl;

class SpeechSynthesizer
{
public:
    SpeechSynthesizer();

    QByteArray synthesize(const QString &data);

private:
    QSharedPointer<SpeechSynthesizerImpl> impl;

};

class SynthesizeException : public std::exception
{
public:
    virtual ~SynthesizeException()
    {}

};

#ifndef NO_AQUESTALK
class AquesTalkSynthesizeException : public SynthesizeException
{};

class TooLongInputException : public AquesTalkSynthesizeException
{
public:
    const char *what() const noexcept;
};
#endif // NO_AQUESTALK

#endif // SPEECHSYNTHESIZER_H
