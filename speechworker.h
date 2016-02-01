#ifndef SPEECHWORKER_H
#define SPEECHWORKER_H

#include <QObject>
#include <QSharedPointer>

#if QT_VERSION >= 0x050000
#include <QAudio>
#include <QAudioFormat>

class QAudioDecoder;
class QAudioOutput;
class QBuffer;
class QIODevice;
#endif

class SpeechWorker : public QObject
{
    Q_OBJECT
public:
    SpeechWorker(const QByteArray &data, const QAudioFormat &format);
    ~SpeechWorker();

    QAudio::State state() const;

signals:
    void ready();

public slots:
    void start();
    void play();
    void resume();
    void stop();

private:
#if QT_VERSION >= 0x050000
    QByteArray data;
    QSharedPointer<QAudioDecoder> decoder;
    QSharedPointer<QAudioOutput> output;
    QSharedPointer<QBuffer> buffer;
    QIODevice *outputDevice;
#endif
};

#endif // SPEECHWORKER_H
