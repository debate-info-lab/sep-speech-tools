#include "utility.h"

#if ! defined(NDEBUG)
#include <QDebug>
#endif

#include <QStringList>
#include <QTextCodec>

#include <mecab.h>

int calcSpeechCount(QString speech)
{
    const QString katakana = QString::fromUtf8(u8"[\u30a1-\u30fc]+");
    if ( QRegExp(katakana).exactMatch(speech) ) {
        const QString youon_kigou = QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30c3\u30e3\u30e5\u30e7\u30fb]");
        speech.remove(QRegExp(youon_kigou));
        return speech.size();
    } else {
        // TODO:
        qDebug() << speech;
        return speech.size();
    }
}

int nodeToSpeechCount(const MeCab::Node *node)
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
    if ( features[0] == QString::fromUtf8(u8"\u8a18\u53f7") ) {
        // IDEOGRAPHIC FULL STOP special case
        if ( surface == QString::fromUtf8(u8"\u3002") ) {
            return 1;
        }
        return 0;
    }
    if ( features.size() < 9 ) {
        return calcSpeechCount(surface);
    } else {
        QString speech = features[8];
        return calcSpeechCount(speech);
    }
}

int getSpeechCount(const QSharedPointer<MeCab::Tagger> &tagger, const QString &text)
{
    if ( ! tagger ) {
        return -1;
    }

    QByteArray utf8 = text.toUtf8();
    const MeCab::Node *node = tagger->parseToNode(utf8.constData(), utf8.size());

    int ret = 0;
    while ( node ) {
        ret += nodeToSpeechCount(node);
        node = node->next;
    }
    return ret;
}


QByteArray toLocalEncoding(const QString &string)
{
#if defined(Q_OS_LINUX)
    return toUtf8Encoding(string);
#elif defined(Q_OS_WIN32) || defined(Q_OS_CYGWIN)
    return toSJisEncoding(string);
#endif
}


QByteArray toUtf8Encoding(const QString &string)
{
    return string.toUtf8();
}


QByteArray toSJisEncoding(const QString &string)
{
    QTextCodec *codec = QTextCodec::codecForName("ShiftJIS");
    return codec->fromUnicode(string);
}
