#include "speechcounter.h"

#include <cassert>

#if ! defined(NDEBUG)
#include <QDebug>
#endif

#include <QRegExp>
#include <QString>
#include <QStringList>

#include "speechoptimizer.h"

SpeechCounter::SpeechCounter(MeCab::Tagger *tagger, const QString &sentence, QObject *parent) :
    QObject(parent),
    optimizer(new SpeechOptimizer(tagger)),
    sentence(sentence),
    nodes()
{
    this->nodes = this->optimizer->parse(this->heuristicNormalize(this->sentence));
}

SpeechCounter::~SpeechCounter()
{
}

double SpeechCounter::getSpeechCount()
{
    if ( this->nodes.isEmpty() ) {
        return -1;
    }
    return this->optimizer->calcSpeechCount(this->nodes);
}

QString SpeechCounter::toRubyHtml()
{
    QString result = "<head>" \
                     "<style>" \
                     "body { font-size: large; }" \
                     ".no-ruby { background-color: red; color: white; }" \
                     ".kigou { background-color: grey; color: white; }" \
                     ".heuristic { background-color: yellow; color: red; }" \
                     "</style>" \
                     "</head>" \
                     "<body>";
    result += this->optimizer->toRubyHtml(this->nodes);
    result += "</body>";
    return result;
}

QString SpeechCounter::toSpeech()
{
    return this->optimizer->toSpeech(this->nodes);
}

QString SpeechCounter::toSpeechForAquesTalk()
{
    return this->optimizer->toSpeechForAquesTalk(this->nodes);
}

QString SpeechCounter::heuristicNormalize(QString sentence) const
{
    sentence = sentence.normalized(QString::NormalizationForm_KC);

    QString result;
    QRegExp numComma("(\\d),(\\d)");

    int prev = 0;
    int i = sentence.indexOf(numComma);
    while ( i > -1 ) {
        result += sentence.mid(prev, i - prev);
        result += numComma.cap(1) + numComma.cap(2);
        prev = i + numComma.cap().size();
        i = sentence.indexOf(numComma, prev);
    }
    result += sentence.mid(prev);
    return result;
}
