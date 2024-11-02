#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QObject>
#include <QBuffer>
#include <QByteArray>
#include <QAudioSource>
#include <QaudioFormat>
#include <QMediaDevices>
#include <QDebug>
#include <opus.h>

class AudioInput : public QIODevice {
    Q_OBJECT

public:
    AudioInput(QObject *parent = nullptr);
    ~AudioInput();

    // qint64 writeData(const char *data, qint64 len) override;

    //void start();
    //void stop();

private:
    QAudioSource *audioSource;
    QByteArray audioBuffer;
    int bufferSize;
    OpusEncoder *encoder;
    int frameSize;

    // qint64 encodeAudio(const char *data, qint64 len);

protected:
    qint64 readData (char* data, qint64 maxLen) override;

    qint64 writeData (const char* data, qint64 len) override;

signals:
    void bufferIsReady(const QByteArray &buffer);


};

#endif // AUDIOINPUT_H
