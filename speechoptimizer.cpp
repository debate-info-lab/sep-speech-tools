#include "speechoptimizer.h"

#include <QDebug>

static QMap<QChar, QString> HEURISTIC_MAP;
static bool HEURISTIC_MAP_INITIALIZED = false;

SpeechOptimizer::SpeechOptimizer() :
    KIGOU(QString::fromUtf8(u8"\u8a18\u53f7")),
    TOUTEN(QString::fromUtf8(u8"\u3001")),
    KUTEN(QString::fromUtf8(u8"\u3002")),
    PERCENT("%"),
    katakana(QString::fromUtf8(u8"[\u30a1-\u30fc]+")),
    youon_kigou(QString::fromUtf8(u8"[\u30a1\u30a3\u30a5\u30a7\u30a9\u30e3\u30e5\u30e7\u30ee\u30fb]")),
    sokuon(QString::fromUtf8(u8"[\u30c3]")),
    ascii("[ -~]")
{
}

double SpeechOptimizer::calcSpeechCount(const MeCabNode &node)
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

QString SpeechOptimizer::toRubyHtml(const MeCabNode &node)
{
    // using classes:
    //   .kigou
    //   .no-ruby

    QString surface = node.surface();
    // counted special case
    if ( surface == this->TOUTEN || surface == this->KUTEN ) {
        return node.surface();
    } else if ( surface == this->PERCENT ) {
        return QString::fromUtf8("<ruby>%1<rt>\u30d1\u30fc\u30bb\u30f3\u30c8</rt></ruby>").arg(node.surface());
    // uncounted special case
    } else if ( node.parts() == this->KIGOU ) {
        return QString("<span class=\"kigou\">%1</span>").arg(node.surface());
    // speech
    } else if ( node.hasSpeech() ) {
        return QString("<ruby>%1<rt>%2</rt></ruby>").arg(node.surface()).arg(node.speech());
    // unknown
    } else {
        return QString("<span class=\"no-ruby\">%1</span>").arg(node.surface());
    }
}

QString SpeechOptimizer::toSpeech(const MeCabNode &node)
{
    if ( node.hasSpeech() && node.speech() != "*" && node.parts() != this->KIGOU ) {
        return node.speech();
    } else if ( node.surface() == this->TOUTEN || node.surface() == this->KUTEN ) {
        return node.speech();
    } else {
        return this->heuristicSpeech(node.surface());
    }
}

double SpeechOptimizer::calcSpeechCount(QString speech)
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

QString SpeechOptimizer::heuristicSpeech(const QString &surface) const
{
    this->heuristicInitialize();

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
