#include "speechcounter.h"

#include <cassert>

#if ! defined(NDEBUG)
#include <QDebug>
#endif

#include <QRegExp>
#include <QString>
#include <QStringList>

static QMap<QChar, QString> HEURISTIC_MAP;
static bool HEURISTIC_MAP_INITIALIZED = false;

SpeechCounter::SpeechCounter(MeCab::Tagger *tagger, const QString &sentence, QObject *parent) :
    QObject(parent),
    KIGOU(QString::fromUtf8(u8"\u8a18\u53f7")),
    TOUTEN(QString::fromUtf8(u8"\u3001")),
    KUTEN(QString::fromUtf8(u8"\u3002")),
    PERCENT("%"),
    katakana(QString::fromUtf8(u8"[\u30a1-\u30fc]+")),
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30e3\u30e5\u30e7\u30ee\u30fb]")),
    sokuon(QString::fromUtf8(u8"[\u30c3]")),
    ascii("[ -~]"),
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
                     ".kigou { background-color: grey; color: white; }" \
                     "</style>" \
                     "</head>" \
                     "<body>";
    for ( const MeCabNode &item : this->nodes ) {
        QString surface = item.surface();
        // counted special case
        if ( surface == this->TOUTEN || surface == this->KUTEN ) {
            result += item.surface();
        } else if ( surface == this->PERCENT ) {
            result += QString::fromUtf8("<ruby>%1<rt>\u30d1\u30fc\u30bb\u30f3\u30c8</rt></ruby>").arg(item.surface());
        // uncounted special case
        } else if ( item.parts() == this->KIGOU ) {
            result += QString("<span class=\"kigou\">%1</span>").arg(item.surface());
        // speech
        } else if ( item.hasSpeech() ) {
            result += QString("<ruby>%1<rt>%2</rt></ruby>").arg(item.surface()).arg(item.speech());
        // unknown
        } else {
            result += QString("<span class=\"no-ruby\">%1</span>").arg(item.surface());
        }
    }
    result += "</body>";
    return result;
}

QString SpeechCounter::toSpeech() const
{
    QString result;
    for ( const MeCabNode &item : this->nodes ) {
        if ( item.hasSpeech() && item.speech() != "*" && item.parts() != this->KIGOU ) {
            result += item.speech();
        } else if ( item.surface() == this->TOUTEN || item.surface() == this->KUTEN ) {
            result += item.speech();
        } else {
            result += this->heuristicSpeech(item.surface());
        }
    }
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
        // FULLWIDTH PERCENT SIGN special case
        } else if ( node.surface() == this->PERCENT ) {
            return 5;
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

void SpeechCounter::heuristicInitialize() const
{
    if ( HEURISTIC_MAP_INITIALIZED ) {
        return;
    }

    HEURISTIC_MAP[' '] = QString();
    HEURISTIC_MAP['"'] = QString();
    HEURISTIC_MAP['#'] = QString();
    HEURISTIC_MAP['$'] = QString::fromUtf8(u8"\u30c9\u30eb");
    HEURISTIC_MAP['%'] = QString::fromUtf8(u8"\u30d1\u30fc\u30bb\u30f3\u30c8");
    HEURISTIC_MAP['&'] = QString::fromUtf8(u8"\u30a2\u30f3\u30c9");
    HEURISTIC_MAP['\''] = QString();
    HEURISTIC_MAP['('] = QString();
    HEURISTIC_MAP[')'] = QString();
    HEURISTIC_MAP['*'] = QString();
    HEURISTIC_MAP['+'] = QString::fromUtf8(u8"\u30d7\u30e9\u30b9");
    HEURISTIC_MAP[','] = QString();
    HEURISTIC_MAP['-'] = QString::fromUtf8(u8"\u30de\u30a4\u30ca\u30b9");
    HEURISTIC_MAP['.'] = QString();
    HEURISTIC_MAP['/'] = QString::fromUtf8(u8"\u30b9\u30e9\u30c3\u30b7\u30e5");

    HEURISTIC_MAP['0'] = QString::fromUtf8(u8"\u30bc\u30ed");
    HEURISTIC_MAP['1'] = QString::fromUtf8(u8"\u30a4\u30c1");
    HEURISTIC_MAP['2'] = QString::fromUtf8(u8"\u30cb\u30fc");
    HEURISTIC_MAP['3'] = QString::fromUtf8(u8"\u30b5\u30f3");
    HEURISTIC_MAP['4'] = QString::fromUtf8(u8"\u30e8\u30f3");
    HEURISTIC_MAP['5'] = QString::fromUtf8(u8"\u30b4\u30fc");
    HEURISTIC_MAP['6'] = QString::fromUtf8(u8"\u30ed\u30af");
    HEURISTIC_MAP['7'] = QString::fromUtf8(u8"\u30ca\u30ca");
    HEURISTIC_MAP['8'] = QString::fromUtf8(u8"\u30cf\u30c1");
    HEURISTIC_MAP['9'] = QString::fromUtf8(u8"\u30ad\u30e5\u30fc");

    HEURISTIC_MAP[':'] = QString();
    HEURISTIC_MAP[';'] = QString();
    HEURISTIC_MAP['<'] = QString::fromUtf8(u8"\u30b7\u30e7\u30a6\u30ca\u30ea");
    HEURISTIC_MAP['='] = QString::fromUtf8(u8"\u30a4\u30b3\u30fc\u30eb");
    HEURISTIC_MAP['>'] = QString::fromUtf8(u8"\u30c0\u30a4\u30ca\u30ea");
    HEURISTIC_MAP['?'] = QString();
    HEURISTIC_MAP['@'] = QString::fromUtf8(u8"\u30a2\u30c3\u30c8");

    HEURISTIC_MAP['a'] = QString::fromUtf8(u8"\u30a8\u30fc");
    HEURISTIC_MAP['b'] = QString::fromUtf8(u8"\u30d3\u30fc");
    HEURISTIC_MAP['c'] = QString::fromUtf8(u8"\u30b7\u30fc");
    HEURISTIC_MAP['d'] = QString::fromUtf8(u8"\u30c7\u30a3\u30fc");
    HEURISTIC_MAP['e'] = QString::fromUtf8(u8"\u30a4\u30fc");
    HEURISTIC_MAP['f'] = QString::fromUtf8(u8"\u30a8\u30d5");
    HEURISTIC_MAP['g'] = QString::fromUtf8(u8"\u30b8\u30fc");
    HEURISTIC_MAP['h'] = QString::fromUtf8(u8"\u30a8\u30a4\u30c1");
    HEURISTIC_MAP['i'] = QString::fromUtf8(u8"\u30a2\u30a4");
    HEURISTIC_MAP['j'] = QString::fromUtf8(u8"\u30b8\u30a7\u30a4");
    HEURISTIC_MAP['k'] = QString::fromUtf8(u8"\u30b1\u30a4");
    HEURISTIC_MAP['l'] = QString::fromUtf8(u8"\u30a8\u30eb");
    HEURISTIC_MAP['m'] = QString::fromUtf8(u8"\u30a8\u30e0");
    HEURISTIC_MAP['n'] = QString::fromUtf8(u8"\u30a8\u30cc");
    HEURISTIC_MAP['o'] = QString::fromUtf8(u8"\u30aa\u30fc");
    HEURISTIC_MAP['p'] = QString::fromUtf8(u8"\u30d4\u30fc");
    HEURISTIC_MAP['q'] = QString::fromUtf8(u8"\u30ad\u30e5\u30fc");
    HEURISTIC_MAP['r'] = QString::fromUtf8(u8"\u30a2\u30fc\u30eb");
    HEURISTIC_MAP['s'] = QString::fromUtf8(u8"\u30a8\u30b9");
    HEURISTIC_MAP['t'] = QString::fromUtf8(u8"\u30c6\u30a3\u30fc");
    HEURISTIC_MAP['u'] = QString::fromUtf8(u8"\u30e6\u30fc");
    HEURISTIC_MAP['v'] = QString::fromUtf8(u8"\u30d6\u30a4");
    HEURISTIC_MAP['w'] = QString::fromUtf8(u8"\u30c0\u30d6\u30ea\u30e5\u30fc");
    HEURISTIC_MAP['x'] = QString::fromUtf8(u8"\u30a8\u30c3\u30af\u30b9");
    HEURISTIC_MAP['y'] = QString::fromUtf8(u8"\u30ef\u30a4");
    HEURISTIC_MAP['z'] = QString::fromUtf8(u8"\u30bc\u30c3\u30c8");

    HEURISTIC_MAP['['] = QString();
    HEURISTIC_MAP['\\'] = QString();
    HEURISTIC_MAP[']'] = QString();
    HEURISTIC_MAP['^'] = QString();
    HEURISTIC_MAP['_'] = QString();
    HEURISTIC_MAP['`'] = QString();

    HEURISTIC_MAP['{'] = QString();
    HEURISTIC_MAP['|'] = QString();
    HEURISTIC_MAP['}'] = QString();
    HEURISTIC_MAP['~'] = QString();

    HEURISTIC_MAP_INITIALIZED = true;
}

QString SpeechCounter::heuristicSpeech(const QString &surface) const
{
    if ( ! HEURISTIC_MAP_INITIALIZED ) {
        this->heuristicInitialize();
    }

    QString result;
    for ( QChar ch : surface ) {
        ch = ch.toLower();
        if ( HEURISTIC_MAP.find(ch) != HEURISTIC_MAP.end() ) {
            result += HEURISTIC_MAP[ch];
        } else {
            // TODO:
            result += " ";
        }
    }
    return result;
}
