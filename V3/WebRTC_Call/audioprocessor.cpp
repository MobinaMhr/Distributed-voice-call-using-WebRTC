#include "audioprocessor.h"
#include <QDebug>

AudioProcessor::AudioProcessor(QObject *parent)
    : QObject(parent), opusEncoder(nullptr), opusDecoder(nullptr), opusFrameSize(960), maxPacketSize(4000)  // Default values for Opus
{
}

AudioProcessor::~AudioProcessor()
{
    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
    }
    if (opusDecoder) {
        opus_decoder_destroy(opusDecoder);
    }
}

bool AudioProcessor::initializeEncoder()
{
    int error;
    opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &error);  // Create Opus encoder for VoIP
    if (error != OPUS_OK) {
        qWarning() << "Failed to initialize Opus encoder. Error code:" << error;
        return false;
    }

    return true;
}

bool AudioProcessor::initializeDecoder()
{
    int error;
    opusDecoder = opus_decoder_create(48000, 1, &error);  // Create Opus decoder for 48kHz, mono audio
    if (error != OPUS_OK) {
        qWarning() << "Failed to initialize Opus decoder. Error code:" << error;
        return false;
    }

    return true;
}

void AudioProcessor::encodeAudio(const QByteArray &rawAudioData)
{
    QByteArray encodedData;
    encodedData.resize(maxPacketSize);  // Prepare buffer for encoded data

    // Encode raw PCM audio data using Opus
    int encodedBytes = opus_encode(opusEncoder, reinterpret_cast<const opus_int16*>(rawAudioData.constData()),
                                   opusFrameSize, reinterpret_cast<unsigned char*>(encodedData.data()), maxPacketSize);

    if (encodedBytes > 0) {
        encodedData.resize(encodedBytes);  // Resize to actual encoded data size
        emit audioEncoded(encodedData);    // Emit encoded data for transmission
    } else {
        qWarning() << "Opus encoding failed with error code:" << encodedBytes;
    }
}

void AudioProcessor::decodeAudio(const QByteArray &encodedAudio)
{
    QByteArray decodedData;
    decodedData.resize(opusFrameSize * 2);  // Opus decodes to PCM, 16-bit (2 bytes per sample)

    // Decode Opus-encoded data into PCM audio
    int decodedSamples = opus_decode(opusDecoder, reinterpret_cast<const unsigned char*>(encodedAudio.constData()),
                                     encodedAudio.size(), reinterpret_cast<opus_int16*>(decodedData.data()), opusFrameSize, 0);

    if (decodedSamples > 0) {
        decodedData.resize(decodedSamples * 2);  // 2 bytes per sample (16-bit PCM)
        emit audioDecoded(decodedData);          // Emit decoded data for playback
    } else {
        qWarning() << "Opus decoding failed with error code:" << decodedSamples;
    }
}
