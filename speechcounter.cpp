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
    optimizer(new SpeechOptimizer),
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
        ret += this->optimizer->calcSpeechCount(item);
    }
    return ret;
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
    for ( const MeCabNode &node : this->nodes ) {
        result += this->optimizer->toRubyHtml(node);
    }
    result += "</body>";
    return result;
}

QString SpeechCounter::toSpeech() const
{
    QString result;
    for ( const MeCabNode &node : this->nodes ) {
        result += this->optimizer->toSpeech(node);
    }
    return result;
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
    // from <http://neu101.seesaa.net/article/159968583.html>

    QList<int> nums;
    for ( const QChar &ch : numstr ) {
        assert( ch.toLatin1() - 0x30 >= 0 && ch.toLatin1() - 0x30 <= 9 );
        nums.insert(0, ch.toLatin1() - 0x30);  // ch - '0'
    }
    int mod = (4 - nums.size() % 4) % 4;
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
