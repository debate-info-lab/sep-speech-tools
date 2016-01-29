#ifndef UTILITY_H
#define UTILITY_H

#include <QByteArray>
#include <QSharedPointer>

QByteArray toLocalEncoding(const QString &string);
QByteArray toUtf8Encoding(const QString &string);
QByteArray toSJisEncoding(const QString &string);

#endif // UTILITY_H
