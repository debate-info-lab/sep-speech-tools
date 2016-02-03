#ifndef SPEECHOPTIMIZER_H
#define SPEECHOPTIMIZER_H

#include "mecabnode.h"

class SpeechOptimizer
{
public:
    SpeechOptimizer();

    double calcSpeechCount(const MeCabNode &node);
    QString toRubyHtml(const MeCabNode &node);
    QString toSpeech(const MeCabNode &node);

private:
    double calcSpeechCount(QString speech);

    void heuristicInitialize() const;
    QString heuristicSpeech(const QString &surface) const;

    const QString KIGOU;
    const QString TOUTEN;
    const QString KUTEN;
    const QString PERCENT;

    QRegExp katakana;
    QRegExp youon_kigou;
    QRegExp sokuon;

    QRegExp ascii;

};

#endif // SPEECHOPTIMIZER_H
