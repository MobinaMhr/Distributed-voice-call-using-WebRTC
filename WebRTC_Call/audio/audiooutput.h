#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QAudioSink>
#include <QAudioFormat>
#include <QIODevice>
#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QMediaDevices>
#include <QDebug>
#include <opus.h>
#include "audioprocessor.h"

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    explicit AudioOutput(QObject *parent = nullptr);

    ~AudioOutput();

    void addData(const QByteArray &data);

signals:
    void newPacketGenerated();

private slots:
    void play();

private:
    int decodeAudio(const QByteArray &packet, opus_int16 *outputBuffer);

    QAudioSink *audioSink;

    QIODevice *audioDevice;

    QMutex mutex;

    QQueue<QByteArray> packetQueue;

    int frameSize;

    AudioProcessor *processor;
};

#endif // AUDIOOUTPUT_H
