#ifndef SPEECHCOUNTER_H
#define SPEECHCOUNTER_H

#include <QObject>

#include <QRegExp>

#include <mecab.h>

class SpeechCounter : public QObject
{
    Q_OBJECT
public:
    explicit SpeechCounter(MeCab::Tagger *tagger, QObject *parent = 0);
    ~SpeechCounter();

    int getSpeechCount(const QString &text);

private:
    int nodeToSpeechCount(const MeCab::Node *node) const;
    int calcSpeechCount(QString speech) const;

    QString KIGOU;
    QString KUTEN;

    QRegExp katakana;
    QRegExp youon_kigou;

    MeCab::Tagger *tagger;

};

#endif // SPEECHCOUNTER_H
