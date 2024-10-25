#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QIODevice>
#include <QAudioSource>
#include <QMediaDevices>
#include <QMutex>
#include <QThread>
#include <opus.h>

class AudioInput : public QIODevice {
    Q_OBJECT

public:
    AudioInput(QObject *parent = nullptr);
    ~AudioInput();

    qint64 writeData(const char *data, qint64 len) override;

    void start();
    void stop();

private:
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;

    qint64 processAudio(const char *data, qint64 len);
};

#endif // AUDIOINPUT_H
