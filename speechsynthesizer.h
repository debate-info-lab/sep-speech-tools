#ifndef SPEECHSYNTHESIZER_H
#define SPEECHSYNTHESIZER_H

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

#endif // SPEECHSYNTHESIZER_H
