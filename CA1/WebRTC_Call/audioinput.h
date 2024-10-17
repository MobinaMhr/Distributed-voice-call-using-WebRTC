#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <./../libraries/opus/include/opus.h>  // Include the Opus library
#include <./../libraries/libdatachannel/include/rtc/rtc.hpp>
#include <./../libraries/libdatachannel/include/rtc/datachannel.hpp>

// #include <QAudioFormat>
#include <QIODevice>
#include <QAudioSource>
#include <QDebug>
// #include <QAudioDevice>
// #include <QMediaDevices>


class AudioInput : public QIODevice {
    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    ~AudioInput() override;

    void start();
    void stop();

protected:
    qint64 writeData(const char *data, qint64 len) override;
    qint64 readData(char *data, qint64 maxlen) override;

private:
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;
    rtc::DataChannel *dataChannel;  // WebRTC data channel for sending audio
};

#endif // AUDIOINPUT_H
