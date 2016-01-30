#include "speechcounter.h"

#if ! defined(NDEBUG)
#include <QDebug>
#endif

#include <QRegExp>
#include <QString>
#include <QStringList>

SpeechCounter::SpeechCounter(MeCab::Tagger *tagger, const QString &sentence, QObject *parent) :
    QObject(parent),
    KIGOU(QString::fromUtf8(u8"\u8a18\u53f7")),
    KUTEN(QString::fromUtf8(u8"\u3002")),
    katakana(QString::fromUtf8(u8"[\u30a1-\u30fc]+")),
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30c3\u30e3\u30e5\u30e7\u30fb]")),
    tagger(tagger),
    sentence(sentence),
    nodes()
{
}

SpeechCounter::~SpeechCounter()
{
}

int SpeechCounter::getSpeechCount()
{
    if ( ! this->tagger ) {
        return -1;
    }

    QByteArray utf8 = this->sentence.toUtf8();
    const MeCab::Node *node = this->tagger->parseToNode(utf8.constData(), utf8.size());
    this->nodes = MeCabNode::create_nodes(node);

    int ret = 0;
    for (const MeCabNode &item : this->nodes) {
        ret += this->nodeToSpeechCount(item);
    }
    return ret;
}

int SpeechCounter::nodeToSpeechCount(const MeCabNode &node) const
{
    // special cases
    if ( node.surface().isEmpty() ) {
        return 0;
    }
    if ( node.feature().startsWith("BOS/EOS") ) {
        return 0;
    }

    //qDebug() << node.surface() << node.feature();
    // check KIGOU
    if ( node.parts() == this->KIGOU ) {
        // IDEOGRAPHIC FULL STOP special case
        if ( node.surface() == this->KUTEN ) {
            return 1;
        }
        return 0;
    }
    if ( node.hasSpeech() ) {
        return this->calcSpeechCount(node.speech());
    } else {
        // no speech
        return this->calcSpeechCount(node.surface());
    }
}

int SpeechCounter::calcSpeechCount(QString speech) const
{
    if ( this->katakana.exactMatch(speech) ) {
        speech.remove(this->youon_kigou);
        return speech.size();
    } else {
        // TODO:
        qDebug() << speech;
        return speech.size();
    }
}
