#ifndef AUDIOPROCESSOR_H
#define AUDIOPROCESSOR_H

#include <QObject>
#include <./../libraries/opus/include/opus.h>
#include <QByteArray>

class AudioProcessor : public QObject
{
    Q_OBJECT

public:
    explicit AudioProcessor(QObject *parent = nullptr);
    ~AudioProcessor();

    // Initialize both encoder and decoder
    bool initializeEncoder();  // Initializes the Opus encoder
    bool initializeDecoder();  // Initializes the Opus decoder

    // Encode raw microphone audio
    void encodeAudio(const QByteArray &rawAudioData);

    // Decode incoming encoded audio
    void decodeAudio(const QByteArray &encodedAudio);

signals:
    void audioEncoded(const QByteArray &encodedData);  // Signal to send encoded data (for transmission)
    void audioDecoded(const QByteArray &decodedData);  // Signal to provide decoded data (for playback)

private:
    // Opus encoder and decoder
    OpusEncoder *opusEncoder;
    OpusDecoder *opusDecoder;

    int opusFrameSize;         // Frame size for Opus encoding/decoding
    int maxPacketSize;         // Maximum packet size for encoding
};

#endif // AUDIOPROCESSOR_H
