#ifndef SPEECHCOUNTER_H
#define SPEECHCOUNTER_H

#include <QObject>
#include <QSharedPointer>

#include <QRegExp>

#include "mecabnode.h"

class SpeechOptimizer;

class SpeechCounter : public QObject
{
    Q_OBJECT
public:
    explicit SpeechCounter(MeCab::Tagger *tagger, const QString &sentence, QObject *parent = 0);
    ~SpeechCounter();

    double getSpeechCount();

    QString toRubyHtml() const;
    QString toSpeech() const;

private:
    QString heuristicNormalize(QString sentence) const;

    QSharedPointer<SpeechOptimizer> optimizer;

    MeCab::Tagger *tagger;
    QString sentence;
    MeCabResult nodes;

};

#endif // SPEECHCOUNTER_H
