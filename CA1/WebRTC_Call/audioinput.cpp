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
    // Convert raw audio data into QByteArray
    QByteArray audioData(data, len);

    if (!audioProcessor->initializeEncoder()) {
        qWarning() << "Failed to initialize the Opus encoder.";
        return 0;
    }

    QByteArray encodedData = audioProcessor->encodeAudio(audioData);

    if (dataChannel && dataChannel->isOpen()) {
        // const char* audioData = reinterpret_cast<const char*>(encodedData.constData());
        dataChannel->send(encodedData.constData());
        return len; // may be the len of encoded data
    } else {
        qWarning() << "Data channel is not open or not initialized.";
        return 0;
    }
}

qint64 AudioInput::readData(char *data, qint64 maxlen) {
    // Not needed for this case (audio input), can return 0
    return 0;
}
