#include "audiooutput.h"

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent), audioSink(nullptr), audioDevice(nullptr), frameSize(960)
{
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format))
        qFatal("Audio format not supported");

    int status;
    opusDecoder = opus_decoder_create(48000, 1, &status);
    if (status != OPUS_OK)
        qFatal("Failed to create Opus decoder: %s", opus_strerror(status));

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

    if (opusDecoder)
        opus_decoder_destroy(opusDecoder);
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
        qWarning() << "AudioOutput(***) No available data to play.";
        return;
    }

    QByteArray packet = packetQueue.dequeue();
    opus_int16 outputBuff[2 * frameSize];

    int packetSize = packet.size();
    if (packetSize < 1) {
        qWarning() << "AudioOutput(***) Invalid encoded packet size: " << packetSize;
        return;
    }

    int decodingResult;
    decodingResult = opus_decode(opusDecoder, reinterpret_cast<const unsigned char *>(packet.data()),
                                 packet.size(), outputBuff, frameSize, 0);

    if (decodingResult < 0) {
        qWarning() << "AudioOutput(***) Decoder process failed. Errof: " << opus_strerror(decodingResult);
        return;
    }

    audioDevice->write(reinterpret_cast<const char*>(outputBuff), decodingResult * sizeof(opus_int16));
}
