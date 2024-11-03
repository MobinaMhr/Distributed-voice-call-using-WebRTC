#include "audioinput.h"

AudioInput::AudioInput(QObject *parent)
    : QIODevice(parent), audioSource(nullptr), encoder(nullptr),
    bufferSize(4096), frameSize(960) {

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

void AudioInput::start()
{
    if (!audioSource) {
        qWarning() << "audioSource doesn't exist" ;
        return;
    }
    if (!this->open(QIODevice::ReadWrite)) {
        qWarning() << "AudioInput: Failed to open QIODevice for writing";
        return;
    }
    audioSource->start(this);
    if (audioSource->state() != QAudio::ActiveState)
        qWarning() << "Audio source is not active. State:" << audioSource->state();

}

void AudioInput::stop()
{
    if (audioSource)
        audioSource->stop();
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
        // qWarning() << "AudioInput: Opus encoding failed, error code:" << encodedBytes;
        return -1;
    }

    QByteArray audioData(reinterpret_cast<const char*>(encodedData), encodedBytes);
    emit bufferIsReady(audioData);

    return len;
}
