#ifndef UTILITY_H
#define UTILITY_H

#include <QByteArray>
#include <QSharedPointer>

namespace MeCab {
class Tagger;
}

int getSpeechCount(const QSharedPointer<MeCab::Tagger> &tagger, const QString &text);


QByteArray toLocalEncoding(const QString &string);
QByteArray toUtf8Encoding(const QString &string);
QByteArray toSJisEncoding(const QString &string);

#endif // UTILITY_H
