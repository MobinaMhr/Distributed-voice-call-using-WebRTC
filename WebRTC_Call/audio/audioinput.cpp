#include "audioinput.h"

AudioInput::AudioInput(QObject *parent)
    : QIODevice(parent), audioSource(nullptr), processor(new AudioProcessor(this)),
    bufferSize(4096), frameSize(960) {

    if (!processor->initializeEncoder()) {
        qWarning() << "AudioInput: Audio processor is not initialized!";
        return;
    }

    QAudioFormat format = AudioProcessor::createAudioFormat();

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

    if (processor)
        delete processor;
}

void AudioInput::start()
{
    if (!audioSource) {
        qWarning() << "AudioInput: audioSource doesn't exist" ;
        return;
    }
    if (!this->open(QIODevice::ReadWrite)) {
        qWarning() << "AudioInput: Failed to open QIODevice for writing";
        return;
    }

    audioSource->start(this);

    if (audioSource->state() != QAudio::ActiveState) {
        qWarning() << "AudioInput: Audio source is not active. State:" << audioSource->state();
    }
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
    const int maxPacketSize = 4000;
    unsigned char encodedData[maxPacketSize];

    int encodedBytes = processor->encodeAudio(data, len / 2, encodedData, maxPacketSize);
    if (encodedBytes < 0) {
        return -1;
    }

    QByteArray audioData(reinterpret_cast<const char*>(encodedData), encodedBytes);
    emit bufferIsReady(audioData);

    return len;
}
