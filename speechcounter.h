#ifndef SPEECHCOUNTER_H
#define SPEECHCOUNTER_H

#include <QObject>

#include <QRegExp>

#include "mecabnode.h"

class SpeechCounter : public QObject
{
    Q_OBJECT
public:
    explicit SpeechCounter(MeCab::Tagger *tagger, const QString &sentence, QObject *parent = 0);
    ~SpeechCounter();

    double getSpeechCount();

    QString toRubyHtml() const;

private:
    double nodeToSpeechCount(const MeCabNode &node) const;
    double calcSpeechCount(QString speech) const;

    const QString KIGOU;
    const QString KUTEN;

    QRegExp katakana;
    QRegExp youon_kigou;
    QRegExp sokuon;

    MeCab::Tagger *tagger;
    QString sentence;
    MeCabResult nodes;

};

#endif // SPEECHCOUNTER_H
