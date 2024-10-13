#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QObject>
#include <QAudioSource>
#include <QIODevice>
#include <QByteArray>

class AudioInput : public QObject
{
    Q_OBJECT
public:
    explicit AudioInput(QObject *parent = nullptr);
    ~AudioInput();

    Q_INVOKABLE void start();

    Q_INVOKABLE void stop();

signals:
    void audioDataReady(const QByteArray &rawAudioData);

private slots:
    void handleAudioInput();

private:
    QAudioSource *audioSource;

    QIODevice *audioIODevice;
};

#endif // AUDIOINPUT_H
