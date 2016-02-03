#ifndef SPEECHWORKER_H
#define SPEECHWORKER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

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

    void start();
    QAudio::State state() const;

    int size() const;

signals:
    void ready();
    void tick(int time);

public slots:
    void play();
    void pause();
    void stop();
    void seek(int time);

private slots:
    void timer_timeout();

private:
    QByteArray data;
    QSharedPointer<QAudioOutput> output;
    QSharedPointer<QBuffer> buffer;
    QTimer timer;

    static const int SAMPLE_SIZE = 16;

};

#endif // NO_MULTIMEDIA

#endif // SPEECHWORKER_H
