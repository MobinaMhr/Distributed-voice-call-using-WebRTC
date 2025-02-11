#include "audioprocessor.h"

AudioProcessor::AudioProcessor(QObject *parent) :
    encoder(nullptr), decoder(nullptr) {}

AudioProcessor::~AudioProcessor() {
    if (decoder) {
        opus_decoder_destroy(decoder);
        decoder = nullptr;
    }

    if (encoder) {
        opus_encoder_destroy(encoder);
        encoder = nullptr;
    }
}

QAudioFormat AudioProcessor::createAudioFormat() {
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    return format;
}

bool AudioProcessor::initializeDecoder() {
    int status;
    decoder = opus_decoder_create(48000, 1, &status);
    if (status != OPUS_OK) {
        qWarning() << "AudioProcessor: Failed to create Opus decoder:" << opus_strerror(status);
        return false;
    }
    return true;
}

bool AudioProcessor::initializeEncoder() {
    int status;
    encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &status);
    if (status != OPUS_OK) {
        qWarning() << "AudioProcessor: Failed to create Opus encoder";
        return false;
    }
    return true;
}

int AudioProcessor::encodeAudio(const char *input, int frameSize, unsigned char* output, int maxPacketSize) {
    if (!encoder) {
        qWarning() << "AudioProcessor: Opus encoder is not initialized!";
        return -1;
    }

    return opus_encode(encoder, reinterpret_cast<const opus_int16*>(input),
                       frameSize, output, maxPacketSize);
}

int AudioProcessor::decodeAudio(const QByteArray *data, opus_int16 *output, int frameSize) {
    if (!decoder) {
        qWarning() << "AudioProcessor: Opus decoder is not initialized!";
        return -1;
    }

    int packetSize = data->size();
    if (packetSize < 1) {
        qWarning() << "AudioProcessor: Invalid encoded packet size:" << packetSize;
        return -1;
    }

    return opus_decode(decoder, reinterpret_cast<const unsigned char *>(data->data()),
                       packetSize, output, frameSize, 0);
}
