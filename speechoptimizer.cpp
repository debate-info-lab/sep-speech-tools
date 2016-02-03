#include "speechoptimizer.h"

#include <cassert>

#include <QDebug>

static QMap<QChar, QString> HEURISTIC_MAP;
static bool HEURISTIC_MAP_INITIALIZED = false;

SpeechOptimizer::SpeechOptimizer(MeCab::Tagger *tagger) :
    tagger(tagger),
    KIGOU(QString::fromUtf8(u8"\u8a18\u53f7")),
    TOUTEN(QString::fromUtf8(u8"\u3001")),
    KUTEN(QString::fromUtf8(u8"\u3002")),
    PERCENT("%"),
    SYOUSUU(QString::fromUtf8(u8"\u5c11\u6570")),
    JOSHI(QString::fromUtf8(u8"\u52a9\u8a5e")),
    KUTOUTEN({this->TOUTEN, this->KUTEN, ",", ".", "!", "?"}),
    katakana(QString::fromUtf8(u8"[\u30a1-\u30fc]+")),
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30e3\u30e5\u30e7\u30ee\u30fb]")),
    sokuon(QString::fromUtf8(u8"[\u30c3]")),
    ascii("[ -~]"),
    nums("\\d+"),
    numKanji(QString::fromUtf8(u8"\u96F6\u4e00\u4e8c\u4e09\u56db\u4e94\u516d\u4e03\u516b\u4e5d")),
    numPlace1(QString::fromUtf8(u8"\u3000\u5341\u767e\u5343")),
    numPlace2(QString::fromUtf8(u8"\u3000\u4e07\u5104\u5146\u4eac\u5793"))
{
}

MeCabResult SpeechOptimizer::parse(const QString &data)
{
    if ( ! this->tagger ) {
        return MeCabResult();
    }

    QByteArray utf8 = data.toUtf8();

    const MeCab::Node *node = this->tagger->parseToNode(utf8.constData(), utf8.size());

    MeCabResult nodes = MeCabNode::create_nodes(node);
    MeCabResult result;
    // TODO: optimize
    int max = nodes.size();
    for ( int i = 0; i < max; ++i ) {
        MeCabNode node = nodes.at(i);
        // for floating point number special case
        if ( this->nums.exactMatch(node.surface()) &&
             i + 2 < max &&
             nodes.at(i+1).surface() == "." &&
             this->nums.exactMatch(nodes.at(i+2).surface()) ) {
            QStringList surfaces = {node.surface(), nodes.at(i+1).surface(), nodes.at(i+2).surface()};
            result.append(MeCabNode::create_dummy(surfaces.join(""), QString::fromUtf8(u8"\u540d\u8a5e,\u5c11\u6570,*,*,*,*,*,,")));
            i += 2;
            continue;
        }
        result.append(node);
    }
    return result;
}

double SpeechOptimizer::calcSpeechCount(const MeCabResult &nodes)
{
    double ret = 0;
    for (const MeCabNode &node : nodes) {
        ret += this->calcSpeechCount(node);
    }
    return ret;
}

double SpeechOptimizer::calcSpeechCount(const MeCabNode &node)
{
    QString surface = node.surface();

    // special cases
    if ( surface.isEmpty() ) {
        return 0;
    }
    if ( surface.startsWith("BOS/EOS") ) {
        return 0;
    }

    //qDebug() << node.surface() << node.feature();
    if ( surface == this->KUTEN ) {
        // IDEOGRAPHIC FULL STOP special case
        return 1;
    } else if ( surface == this->PERCENT ) {
        // PERCENT SIGN special case
        return this->calcSpeechCount(this->heuristicSpeech(surface));
    } else if ( node.parts() == this->KIGOU ) {
        // skip other symbol chars
        return 0;
    }

    QString speech = this->toSpeech(node);
    if ( speech.trimmed().isEmpty() ) {
        return this->calcSpeechCount(surface);
    }
    return this->calcSpeechCount(speech);
}

QString SpeechOptimizer::toRubyHtml(const MeCabResult &nodes)
{
    QString result;
    for ( const MeCabNode &node : nodes ) {
        result += this->toRubyHtml(node);
    }
    return result;
}

QString SpeechOptimizer::toRubyHtml(const MeCabNode &node)
{
    // using classes:
    //   .kigou
    //   .no-ruby
    //   .heuristic

    QString surface = node.surface();
    QString speech = this->toSpeech(node);
    if ( this->KUTOUTEN.indexOf(surface) != -1 ) {
        // counted special case
        return surface;
    } else if ( surface != this->PERCENT && node.parts() == this->KIGOU ) {
        // uncounted special case
        return QString("<span class=\"kigou\">%1</span>").arg(surface);
    } else if ( node.hasSpeech() ) {
        // speech
        return QString("<ruby>%1<rt>%2</rt></ruby>").arg(surface).arg(node.speech());
    } else if ( ! speech.isEmpty() ) {
        // heuristic
        return QString("<ruby class=\"heuristic\">%1<rt>%2</rt></ruby>").arg(surface).arg(speech);
    } else {
        // unknown
        return QString("<span class=\"no-ruby\">%1</span>").arg(surface);
    }
}

QString SpeechOptimizer::toSpeech(const MeCabResult &nodes)
{
    QString result;
    for ( const MeCabNode &node : nodes ) {
        result += this->toSpeech(node);
    }
    return result;
}

QString SpeechOptimizer::toSpeech(const MeCabNode &node)
{
    if ( node.hasSpeech() && node.parts() != this->KIGOU ) {
        // speech (not kigou)
        return node.speech();
    } else if ( this->KUTOUTEN.indexOf(node.surface()) != -1 ) {
        // kutouten special case
        return node.speech();
    } else if ( this->nums.exactMatch(node.surface()) ) {
        // unparsed number case
        QString kanji = this->numToKanji(node.surface());
        return this->toSpeech(this->parse(kanji));
    } else if ( node.parts_detail1() == this->SYOUSUU ) {
        // floating point num
        return this->heuristicSpeechForFloatingPoint(node.surface());
    } else {
        // unknown
        return this->heuristicSpeech(node.surface());
    }
}

#ifndef NO_AQUESTALK

QString SpeechOptimizer::toSpeechForAquesTalk(const MeCabResult &nodes)
{
    QString result;
    int size = nodes.size();
    for ( int i = 0; i < size; ++i ) {
        const MeCabNode &node = nodes.at(i);
        qDebug() << node.surface() << node.feature();
        result += this->toSpeechForAquesTalk(node);
        if ( node.parts() == this->JOSHI &&
             i + 1 < size &&
             this->KUTOUTEN.indexOf(nodes.at(i+1).surface()) == -1 ) {
            result += "+";
        }
    }
    return result;
}

QString SpeechOptimizer::toSpeechForAquesTalk(const MeCabNode &node)
{
    QString surface = node.surface();
    QString speech = this->toSpeech(node);
    if ( surface == "." || surface == "!" || surface == "?" ) {
        speech = this->KUTEN;
    } else if ( node.parts() == this->KIGOU && this->KUTOUTEN.indexOf(surface) == -1 ) {
        speech = this->TOUTEN;
    }
    return speech;
}

#endif // NO_AQUESTALK

double SpeechOptimizer::calcSpeechCount(QString speech)
{
    if ( this->katakana.exactMatch(speech) ) {
        // all katakana case
        speech.remove(this->youon_kigou);
        int half_char = speech.split(this->sokuon).size() - 1;
        return speech.size() - half_char + 0.5 * half_char;
    } else {
        // other case
        // TODO:
        qDebug() << speech;
        return speech.size();
    }
}

void SpeechOptimizer::heuristicInitialize() const
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

QString SpeechOptimizer::heuristicSpeech(const QString &surface, const QMap<QChar, QString> &special) const
{
    this->heuristicInitialize();

    QString result;
    for ( QChar ch : surface ) {
        ch = ch.toLower();
        if ( special.size() > 0 && special.find(ch) != special.end() ) {
            result += special[ch];
        } else if ( HEURISTIC_MAP.find(ch) != HEURISTIC_MAP.end() ) {
            result += HEURISTIC_MAP[ch];
        } else {
            // TODO:
            result += " ";
        }
    }
    return result;
}

QString SpeechOptimizer::heuristicSpeechForFloatingPoint(const QString &surface) const
{
    QMap<QChar, QString> special = {{'.', QString::fromUtf8(u8"\u30c6\u30f3")}};
    return this->heuristicSpeech(surface, special);
}

QString SpeechOptimizer::numToKanji(const QString &numstr) const
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
