#include "audioinput.h"

AudioInput::AudioInput(QObject *parent)
    : QIODevice(parent), audioSource(nullptr), opusEncoder(nullptr) {
    int sampleRate = 48000;
    int channelCount = 1;

    QMediaDevices mediaDevices;
    QAudioDevice inputDevice;

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);         // 48kHz
    audioFormat.setChannelCount(channelCount);     // Mono audio
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    inputDevice = mediaDevices.defaultAudioInput();
    if (!inputDevice.isFormatSupported(audioFormat)) {
        qDebug() << "AudioInput:: Audio format is not supported.";
    }

    audioSource = new QAudioSource(inputDevice, audioFormat, this);
    if (!audioSource) {
        qDebug() << "AudioInput:: Audio format is not supported.";
    }

    int error; // may be OPUS_APPLICATION_VOIP
    opusEncoder = opus_encoder_create(sampleRate, channelCount, OPUS_APPLICATION_AUDIO, &error);
    if (error != OPUS_OK) {
        qDebug() << "AudioInput:: Failed to create Opus encoder:" << opus_strerror(error);
    }
}

AudioInput::~AudioInput() {
    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
    }
    if (audioSource) {
        delete audioSource;
    }
}

void AudioInput::start() {
    audioIODevice = audioSource->start();

    if (audioIODevice) {
        connect(audioIODevice, &QIODevice::readyRead, this, &AudioInput::start);
        qDebug() << "Audio input started.";
    } else {
        qWarning() << "Failed to start audio input.";
    }
}

void AudioInput::stop() {
    if (audioIODevice) {
        disconnect(audioIODevice, &QIODevice::readyRead, this, &AudioInput::start);
        audioSource->stop();
        qDebug() << "Audio input stopped.";
    } else {
        qWarning() << "No audio input to stop.";
    }
}

qint64 AudioInput::writeData(const char *data, qint64 len) {
    qint64 encodedBytes;

    if (!opusEncoder) {
        qDebug() << "AudioInput:: Opus Encoder is not initialized.";
        return -1;
    }

    encodedBytes = encodeAudio(data, len);
    if (encodedBytes < 0) {
        qDebug() << "AudioInput:: opus encoding process failed. ";
        qDebug() << "AudioInput:: Error code " << encodedBytes;
        return -1;
    }

    return len;
}

qint64 AudioInput::encodeAudio(const char *data, qint64 len) {
    char encodedData[4000];

    return opus_encode(
        opusEncoder,
        reinterpret_cast<const opus_int16*>(data),
        len / 2,
        reinterpret_cast<unsigned char*>(encodedData), // Could be simply encodedData
        sizeof(encodedData)
    );
}
