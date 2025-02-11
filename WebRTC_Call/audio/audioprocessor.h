#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include <QAudioFormat>
#include <opus.h>
#include <QDebug>
#include <QByteArray>

class AudioProcessor {

public:
    explicit AudioProcessor(QObject *parent = nullptr);
    ~AudioProcessor();

    static QAudioFormat createAudioFormat();

    bool initializeDecoder();
    bool initializeEncoder();

    int encodeAudio(const char *input, int frameSize, unsigned char* output, int maxPacketSize);
    int decodeAudio(const QByteArray *data, opus_int16 *output, int frameSize);

private:
    OpusEncoder* encoder = nullptr;
    OpusDecoder *decoder = nullptr;
};

#endif // AUDIOPROCESSOR_H
