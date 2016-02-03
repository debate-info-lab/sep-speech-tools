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
    tagger(tagger),
    sentence(sentence),
    nodes()
{
}

SpeechCounter::~SpeechCounter()
{
}

double SpeechCounter::getSpeechCount()
{
    if ( ! this->tagger ) {
        return -1;
    }

    QByteArray utf8 = this->heuristicNormalize(this->sentence).toUtf8();
    const MeCab::Node *node = this->tagger->parseToNode(utf8.constData(), utf8.size());
    this->nodes = MeCabNode::create_nodes(node);

    return this->optimizer->calcSpeechCount(this->nodes);
}

QString SpeechCounter::toRubyHtml() const
{
    QString result = "<head>" \
                     "<style>" \
                     "body { font-size: large; }" \
                     ".no-ruby { background-color: red; color: white; }" \
                     ".kigou { background-color: grey; color: white; }" \
                     "</style>" \
                     "</head>" \
                     "<body>";
    result += this->optimizer->toRubyHtml(this->nodes);
    result += "</body>";
    return result;
}

QString SpeechCounter::toSpeech() const
{
    return this->optimizer->toSpeech(this->nodes);
}

QString SpeechCounter::heuristicNormalize(QString sentence) const
{
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
