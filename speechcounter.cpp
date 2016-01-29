#include "speechcounter.h"

#if ! defined(NDEBUG)
#include <QDebug>
#endif

#include <QRegExp>
#include <QString>
#include <QStringList>

SpeechCounter::SpeechCounter(MeCab::Tagger *tagger, QObject *parent) :
    QObject(parent),
    KIGOU(QString::fromUtf8(u8"\u8a18\u53f7")),
    KUTEN(QString::fromUtf8(u8"\u3002")),
    katakana(QString::fromUtf8(u8"[\u30a1-\u30fc]+")),
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30c3\u30e3\u30e5\u30e7\u30fb]")),
    tagger(tagger)
{
}

SpeechCounter::~SpeechCounter()
{
}

int SpeechCounter::getSpeechCount(const QString &text)
{
    if ( ! this->tagger ) {
        return -1;
    }

    QByteArray utf8 = text.toUtf8();
    const MeCab::Node *node = this->tagger->parseToNode(utf8.constData(), utf8.size());

    int ret = 0;
    while ( node ) {
        ret += this->nodeToSpeechCount(node);
        node = node->next;
    }
    return ret;
}

int SpeechCounter::nodeToSpeechCount(const MeCab::Node *node) const
{
    if ( ! node ) {
        return 0;
    }

    QString surface = QString::fromUtf8(node->surface, node->length);
    QString feature = QString::fromUtf8(node->feature);

    // special cases
    if ( feature.startsWith("BOS/EOS") ) {
        return 0;
    }

    //qDebug() << surface << feature;
    QStringList features = feature.split(",");
    // check KIGOU
    if ( features[0] == this->KIGOU ) {
        // IDEOGRAPHIC FULL STOP special case
        if ( surface == this->KUTEN ) {
            return 1;
        }
        return 0;
    }
    if ( features.size() < 9 ) {
        // no speech
        return this->calcSpeechCount(surface);
    } else {
        // has speech
        QString speech = features[8];
        return this->calcSpeechCount(speech);
    }
}

int SpeechCounter::calcSpeechCount(QString speech) const
{
    if ( QRegExp(this->katakana).exactMatch(speech) ) {
        speech.remove(QRegExp(this->youon_kigou));
        return speech.size();
    } else {
        // TODO:
        qDebug() << speech;
        return speech.size();
    }
}
