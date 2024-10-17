#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

// ./../libraries/opus/include/

// D:/Computer-Networks-Course-S2025/V3/libraries/libdatachannel

#include <D:/Computer-Networks-Course-S2025/V3/libraries/libdatachannel/include/rtc/rtc.hpp>
#include <D:/Computer-Networks-Course-S2025/V3/libraries/libdatachannel/include/rtc/datachannel.hpp>

#include <D:/Computer-Networks-Course-S2025/V3/WebRTC_Call/audioprocessor.h>

#include <QIODevice>
#include <QAudioSource>
#include <QAudioFormat>
#include <QDebug>
#include <QAudioDevice>


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
    AudioProcessor *audioProcessor;
    QIODevice *audioIODevice;
    QAudioSource *audioSource;
    OpusEncoder *opusEncoder;
    rtc::DataChannel *dataChannel;  // WebRTC data channel for sending audio
};

#endif // AUDIOINPUT_H

