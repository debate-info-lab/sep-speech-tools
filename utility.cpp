#include "utility.h"

#include <QStringList>
#include <QTextCodec>


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
