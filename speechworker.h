#ifndef SPEECHWORKER_H
#define SPEECHWORKER_H

#include <QObject>
#include <QSharedPointer>

#if USE_MULTIMEDIA
#include <QAudio>
#include <QAudioFormat>

class QAudioDecoder;
class QAudioOutput;
class QBuffer;
class QIODevice;
#else
#include <QTemporaryFile>

class QMediaPlayer;
#endif

class SpeechWorker : public QObject
{
    Q_OBJECT
public:
#if USE_MULTIMEDIA
    SpeechWorker(const QByteArray &data, const QAudioFormat &format);
#else
    SpeechWorker(const QByteArray &data);
#endif
    ~SpeechWorker();

#if USE_MULTIMEDIA
    QAudio::State state() const;
#endif

signals:
    void ready();

public slots:
#if USE_MULTIMEDIA
    void start();
#endif
    void play();
    void resume();
    void stop();

private:
#if USE_MULTIMEDIA
    QSharedPointer<QAudioDecoder> decoder;
    QSharedPointer<QAudioOutput> output;
    QSharedPointer<QBuffer> buffer;
    QIODevice *outputDevice;
#else
    QSharedPointer<QMediaPlayer> player;
    QTemporaryFile temp;
#endif
    QByteArray data;

};

#endif // SPEECHWORKER_H
