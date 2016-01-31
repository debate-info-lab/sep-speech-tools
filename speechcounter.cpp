#include "speechcounter.h"

#include <cassert>

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
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30e3\u30e5\u30e7\u30ee\u30fb]")),
    sokuon(QString::fromUtf8(u8"[\u30c3]")),
    commas(QString::fromUtf8(u8"[,\ufe50\uff0c]")),
    numKanji(QString::fromUtf8(u8"\uf9b2\u4e00\u4e8c\u4e09\u56db\u4e94\u516d\u4e03\u516b\u4e5d")),
    numPlace1(QString::fromUtf8(u8"\u3000\u5341\u767e\u5343")),
    numPlace2(QString::fromUtf8(u8"\u3000\u4e07\u5104\u5146\u4eac\u5793")),
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

    double ret = 0;
    for (const MeCabNode &item : this->nodes) {
        ret += this->nodeToSpeechCount(item);
    }
    return ret;
}

QString SpeechCounter::toRubyHtml() const
{
    QString result = "<head>" \
                     "<style>" \
                     "body { font-size: large; }" \
                     ".no-ruby { background-color: red; color: white; }" \
                     "</style>" \
                     "</head>" \
                     "<body>";
    for ( const MeCabNode &item : this->nodes ) {
        if ( item.hasSpeech() ) {
            result += QString("<ruby>%1<rt>%2</rt></ruby>").arg(item.surface()).arg(item.speech());
        } else {
            result += QString("<span class=\"no-ruby\">%1</span>").arg(item.surface());
        }
    }
    result += "</body>";
    return result;
}

double SpeechCounter::nodeToSpeechCount(const MeCabNode &node) const
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

double SpeechCounter::calcSpeechCount(QString speech) const
{
    if ( this->katakana.exactMatch(speech) ) {
        speech.remove(this->youon_kigou);
        int half_char = speech.split(this->sokuon).size() - 1;
        return speech.size() - half_char + 0.5 * half_char;
    } else {
        // TODO:
        qDebug() << speech;
        return speech.size();
    }
}

QString SpeechCounter::heuristicNormalize(QString sentence) const
{
    sentence.remove(this->commas);

    QString result;
    QRegExp nums("\\d+");

    // numToKanji
    int prev = 0;
    int i = sentence.indexOf(nums);
    while ( i > -1 ) {
        result += sentence.mid(prev, i - prev);
        result += this->numToKanji(nums.cap());
        prev = i + nums.cap().size();
        i = sentence.indexOf(nums, prev);
    }
    result += sentence.mid(prev);

    return result;
}

QString SpeechCounter::numToKanji(const QString &numstr) const
{
    QList<int> nums;
    for ( const QChar &ch : numstr ) {
        assert( ch.toLatin1() - 0x30 >= 0 && ch.toLatin1() - 0x30 <= 9 );
        nums.insert(0, ch.toLatin1() - 0x30);  // ch - '0'
    }
    int mod = (4 - nums.size() % 4) & 3;
    qDebug() << numstr << "mod:" << mod;
    for ( int i = 0; i < mod; ++i ) {
        nums.append(0);
    }

    QStringList list;
    for ( int i = 0; i < nums.size(); i += 4 ) {
        QStringList temp;
        for ( int k = 0; k < 4; ++k ) {
            int num = nums[i + k];
            if ( num == 0 ) {
                continue;
            }
            QChar place = this->numPlace1.at(k);
            if ( k > 0 && num == 1 ) {
                temp.insert(0, QString(place));
            } else {
                if ( k == 0 ) {
                    temp.insert(0, this->numKanji.at(num));
                } else {
                    temp.insert(0, QString("%1%2").arg(this->numKanji.at(num)).arg(place));
                }
            }
        }
        list.append(temp.join(""));
    }
    if ( list.size() > this->numPlace2.size() ) {
        return numstr;
    }

    QStringList temp;
    for ( int i = 0; i < list.size(); ++i ) {
        QString at = list.at(i);
        if ( at.isEmpty() ) {
            continue;
        }
        if ( at == QString(this->numPlace1.at(3)) ) {
            at = QString("%1%2").arg(this->numKanji.at(1)).arg(at);
        }
        if ( i == 0 ) {
            temp.insert(0, at);
        } else {
            temp.insert(0, QString("%1%2").arg(at).arg(this->numPlace2.at(i)));
        }
    }
    QString result = temp.join("");
    if ( result.isEmpty() ) {
        return this->numKanji.at(0);
    }
    return result;
}
