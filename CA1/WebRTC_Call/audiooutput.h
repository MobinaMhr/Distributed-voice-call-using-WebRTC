#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QObject>
#include <QAudioSink>
#include <QAudioFormat>
#include <QByteArray>
#include <QMediaDevices>
#include <QMutex>
#include <QQueue>
#include <opus.h>
#include <QDebug>

class AudioOutput : public QObject
{
    Q_OBJECT
public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();

    void addData(const QByteArray &data);

signals:
    void newPacket();

private slots:
    void play();

private:
    QAudioSink *audioSink;
    OpusDecoder *opusDecoder;
    QIODevice *audioDevice;
    QMutex mutex;
    QQueue<QByteArray> audioQueue;
};

#endif // AUDIOOUTPUT_H
