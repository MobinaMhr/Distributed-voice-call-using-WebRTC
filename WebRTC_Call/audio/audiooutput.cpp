#include "audiooutput.h"

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent), audioSink(nullptr), audioDevice(nullptr),
    frameSize(960), processor(new AudioProcessor(this))
{
    QAudioFormat format = AudioProcessor::createAudioFormat();

    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format))
        qFatal("Audio format not supported");

    if (!processor->initializeDecoder()) {
        qFatal("Failed to initialize Opus decoder");
    }

    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), format, this);
    audioDevice = audioSink->start();
    if (!audioDevice)
        qWarning() << "AudioOutput: Failed to start audio device";

    connect(this, &AudioOutput::newPacketGenerated, this, &AudioOutput::play);
}

AudioOutput::~AudioOutput()
{
    if (audioSink)
        delete audioSink;

    if (processor)
        delete processor;
}

void AudioOutput::addData(const QByteArray &data)
{
    QMutexLocker locker(&mutex);

    packetQueue.enqueue(data);

    emit newPacketGenerated();
}

void AudioOutput::play()
{
    if (packetQueue.isEmpty()) {
        qWarning() << "AudioOutput: No available data to play.";
        return;
    }

    QByteArray packet = packetQueue.dequeue();
    opus_int16 outputBuff[2 * frameSize];

    int decodingResult = processor->decodeAudio(&packet, outputBuff, frameSize);
    if (decodingResult < 0) {
        qWarning() << "AudioOutput: Decoder process failed.";
        return;
    }

    audioDevice->write(reinterpret_cast<const char*>(outputBuff), decodingResult * sizeof(opus_int16));
}
