#include "audioinput.h"

AudioInput::AudioInput(QObject *parent)
    : QIODevice(parent), audioSource(nullptr), bufferSize(4096),
    encoder(nullptr), frameSize(960) {
    int status;
    encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &status);
    if (status != OPUS_OK) {
        qFatal("Opus encoder creation failed. error : %s", opus_strerror(status));
    }

    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    QMediaDevices mediaDevices;
    QAudioDevice inputDevice;
    inputDevice = mediaDevices.defaultAudioInput();

    if (!inputDevice.isFormatSupported(format)) {
        qFatal("Audio format not supported");
    }

    audioSource = new QAudioSource(inputDevice, format, this);
}

AudioInput::~AudioInput() {
    if (audioSource)
        delete audioSource;
    if (encoder)
        opus_encoder_destroy(encoder);
}

qint64 AudioInput::readData(char *data, qint64 maxLen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxLen);
    return 0;
}

qint64 AudioInput::writeData(const char *data, qint64 len)
{
    if (!encoder) {
        qWarning() << "AudioInput: Opus encoder is not initialized!";
        return -1;
    }

    const int maxPacketSize = 4000;
    unsigned char encodedData[maxPacketSize];

    int encodedBytes = opus_encode(encoder, reinterpret_cast<const opus_int16*>(data), len / 2, encodedData, maxPacketSize);

    if (encodedBytes < 0) {
        qWarning() << "AudioInput: Opus encoding failed, error code:" << encodedBytes;
        return -1;
    }

    QByteArray audioData(reinterpret_cast<const char*>(encodedData), encodedBytes);
    emit bufferIsReady(audioData);
    // qDebug() << "AudioInput: Encoded audio data emitted, length:" << encodedBytes;

    return len;
}

// void AudioInput::start() {
//     if (!audioSource || !open(QIODevice::ReadWrite)) {
//         return;
//     }

//     audioSource->start();

//     if (audioSource->state() != QAudio::ActiveState) {
//         qDebug() << "AudioInput:: Audio source with state " << audioSource->state() << " is not active yet. ";
//     }
// }

// void AudioInput::stop() {
//     if (audioSource) {
//         audioSource->stop();
//     }
//     // we may need to
//     // disconnect(audioIODevice, &QIODevice::readyRead, this, &AudioInput::start);
// }

// qint64 AudioInput::writeData(const char *data, qint64 len) {
//     qint64 encodedBytes;

//     if (!opusEncoder) {
//         qDebug() << "AudioInput:: Opus Encoder is not initialized.";
//         return -1;
//     }

//     encodedBytes = encodeAudio(data, len);
//     if (encodedBytes < 0) {
//         qDebug() << "AudioInput:: opus encoding process failed. ";
//         qDebug() << "AudioInput:: Error code " << encodedBytes;
//         return -1;
//     }

//     return len;
// }

// qint64 AudioInput::encodeAudio(const char *data, qint64 len) {
//     char encodedData[4000];

//     return opus_encode(
//         opusEncoder,
//         reinterpret_cast<const opus_int16*>(data),
//         len / 2,
//         reinterpret_cast<unsigned char*>(encodedData), // Could be simply encodedData
//         sizeof(encodedData)
//     );
// }
