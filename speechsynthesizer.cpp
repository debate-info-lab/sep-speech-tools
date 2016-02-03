#include "speechsynthesizer.h"

#include <QDebug>

class SpeechSynthesizerImpl
{
public:
    virtual ~SpeechSynthesizerImpl()
    {}
    virtual QByteArray synthesize(const QString &data) = 0;

};

#ifndef NO_AQUESTALK

const char *TooLongInputException::what() const noexcept
{
    return "too long input.";
}

#if defined(Q_OS_LINUX)

#include "utility.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus
unsigned char * (AquesTalk_Synthe)(const char *koe, int iSpeed, int *pSize);
void (AquesTalk_FreeWave)(unsigned char *wav);
#ifdef __cplusplus
}
#endif // __cplusplus

class LinuxSpeechSynthesizerImpl : public SpeechSynthesizerImpl
{
public:
    QByteArray synthesize(const QString &data)
    {
        QByteArray indata = toSJisEncoding(data);
        int size = 0;
        unsigned char *ptr = AquesTalk_Synthe(indata.constData(), 100, &size);
        if ( ! ptr ) {
            qDebug() << "Error:" << size;
            if ( size == 200 ) {
                // Too long input
                throw TooLongInputException();
            }
            return QByteArray();
        }
        QByteArray result(reinterpret_cast<char *>(ptr), size);
        AquesTalk_FreeWave(ptr);
        return result;
    }
};

#elif defined(Q_OS_WIN)

#include <QCoreApplication>

#include "windows/dynamiclinklibrary.hpp"

static const QString AQUESTALK_DEFAULT_PATH = "AquesTalk/f1/AquesTalk.dll";

class WindowsSpeechSynthesizerImpl : public SpeechSynthesizerImpl
{
public:
    typedef unsigned char * ( __stdcall *AquesTalk_SyntheDef)(const char *koe, int iSpeed, int *pSize);
    typedef void ( __stdcall *AquesTalk_FreeWaveDef)(unsigned char *wav);

    WindowsSpeechSynthesizerImpl() :
        WindowsSpeechSynthesizerImpl(QCoreApplication::applicationDirPath() + "/" + AQUESTALK_DEFAULT_PATH)
    {}
    WindowsSpeechSynthesizerImpl(const QString &path) :
        library(new DynamicLoadLibrary),
        AquesTalk_Synthe(nullptr),
        AquesTalk_FreeWave(nullptr)
    {
        if ( this->library->initialize(path) ) {
            this->AquesTalk_Synthe = reinterpret_cast<AquesTalk_SyntheDef>(this->library->get("AquesTalk_Synthe"));
            this->AquesTalk_FreeWave = reinterpret_cast<AquesTalk_FreeWaveDef>(this->library->get("AquesTalk_FreeWave"));
        }
    }

    QByteArray synthesize(const QString &data)
    {
        if ( ! this->library->loaded() ||
             ! this->AquesTalk_Synthe ||
             ! this->AquesTalk_FreeWave ) {
            return QByteArray();
        }
        QByteArray indata = toSJisEncoding(data);
        int size = 0;
        unsigned char *ptr = AquesTalk_Synthe(indata.constData(), 100, &size);
        if ( ! ptr ) {
            qDebug() << "Error:" << size;
            if ( size == 200 ) {
                // Too long input
                throw TooLongInputException();
            }
            return QByteArray();
        }
        QByteArray result(reinterpret_cast<char *>(ptr), size);
        AquesTalk_FreeWave(ptr);
        return result;
    }

private:
    QSharedPointer<DynamicLoadLibrary> library;

    AquesTalk_SyntheDef AquesTalk_Synthe;
    AquesTalk_FreeWaveDef AquesTalk_FreeWave;

};

#endif // Q_OS_LINUX | Q_OS_WIN

#endif // NO_AQUESTALK

SpeechSynthesizer::SpeechSynthesizer() :
#if defined(Q_OS_LINUX)
    impl(new LinuxSpeechSynthesizerImpl)
#elif defined(Q_OS_WIN)
    impl(new WindowsSpeechSynthesizerImpl)
#endif
{

}

QByteArray SpeechSynthesizer::synthesize(const QString &data)
{
    if ( ! this->impl ) {
        return QByteArray();
    }
#ifndef NO_AQUESTALK
    QString TOUTEN = QString::fromUtf8(u8"\u3001");
    QString KUTEN = QString::fromUtf8(u8"\u3002");
    QString replaced = data.trimmed().replace(QRegExp("\\s+"), TOUTEN);

    QByteArray result;
    for ( const QString &in : replaced.split(KUTEN) ) {
        QByteArray temp = this->impl->synthesize(in + KUTEN);
        if ( result.isEmpty() ) {
            result = temp;
        } else {
            // 16bit 4096 frame no wave + WAVE header skipped raw data
            result += QByteArray(8192, '\0') + temp.mid(44);
        }
    }
    return result;
#else
    return this->impl->synthesize(data);
#endif
}


