#include "audiooutput.h"

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent), audioSink(nullptr), opusDecoder(nullptr), audioDevice(nullptr) {
    int sampleRate = 48000;
    int channelCount = 1;

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(sampleRate);         // 48kHz
    audioFormat.setChannelCount(channelCount);     // Mono audio
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(audioFormat)) {
        qDebug() << "This audio format is not supported.";
    }
}
