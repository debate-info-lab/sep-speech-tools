#ifndef SPEECHWORKER_H
#define SPEECHWORKER_H

#include <QObject>
#include <QSharedPointer>

#ifndef NO_MULTIMEDIA
#include <QAudio>
#include <QAudioFormat>

class QAudioOutput;
class QBuffer;


class SpeechWorker : public QObject
{
    Q_OBJECT
public:
    SpeechWorker(const QByteArray &data, const QAudioFormat &format);
    ~SpeechWorker();

    QAudio::State state() const;

public slots:
    void play();
    void pause();
    void stop();

private:
    QSharedPointer<QAudioOutput> output;
    QSharedPointer<QBuffer> buffer;
    QByteArray data;

};

#endif // NO_MULTIMEDIA

#endif // SPEECHWORKER_H
