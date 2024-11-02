#include "audiooutput.h"

// AudioOutput::AudioOutput(QObject *parent)
//     : QObject(parent), audioSink(nullptr), opusDecoder(nullptr), audioDevice(nullptr) {
//     int sampleRate = 48000;
//     int channelCount = 1;

//     QAudioFormat audioFormat;
//     audioFormat.setSampleRate(sampleRate);         // 48kHz
//     audioFormat.setChannelCount(channelCount);     // Mono audio
//     audioFormat.setSampleFormat(QAudioFormat::Int16);

//     if (!QMediaDevices::defaultAudioOutput().isFormatSupported(audioFormat)) {
//         qDebug() << "This audio format is not supported.";
//     }

//     audioSink = new QAudioSink(QMediaDevices::defaultAudioInput(), audioFormat, this);

//     int error;
//     opusDecoder = opus_decoder_create(sampleRate, channelCount, &error);
//     if (error != OPUS_OK) {
//         qWarning() << "Failed to create Opus decoder:" << opus_strerror(error);
//     }

//     audioDevice = audioSink->start();
//     if (!audioDevice) {
//         qDebug() << "Failed to start audio device.";
//     }

//     connect(this, &AudioOutput::newPacket, this, &AudioOutput::play);
// }

// AudioOutput::~AudioOutput() {
//     if (audioSink) {
//         delete audioSink;
//     }
//     if (opusDecoder) {
//         opus_decoder_destroy(opusDecoder);
//     }
// }

// void AudioOutput::addData(const QByteArray &data) {
//     QMutexLocker locker(&mutex);

//     qDebug() << "Adding new data to the queue.";
//     qDebug() << "Audio length = " << data.size();

//     audioQueue.enqueue(data);
//     emit newPacket();

//     play();
// }

// void AudioOutput::play() {
//     if (audioQueue.isEmpty()) {
//         qDebug() << "AudioOutput: No data to play. Queue is empty";
//         return;
//     }

//     QByteArray packet = audioQueue.dequeue();

//     if (packet.size() <= 0) {
//         qWarning() << "AudioOutput: Invalid encoded data size";
//         return;
//     }

//     opus_int16 outputBuffer[960 * 2]; // 960 is the frameSize

//     int decodedSamples;
//     decodedSamples = decodeAudio(packet, outputBuffer);
//     if (decodedSamples < 0) {
//         qWarning() << "opus decoder failed" << opus_strerror(decodedSamples);
//         return;
//     }

//     audioDevice->write(reinterpret_cast<const char*>(outputBuffer), decodedSamples * sizeof(opus_int16));
// }

// int AudioOutput::decodeAudio(const QByteArray &packet, opus_int16 *outputBuffer) {
//     return opus_decode(
//         opusDecoder,
//         reinterpret_cast<const unsigned char *>(packet.data()),
//         packet.size(),
//         outputBuffer,
//         960,
//         0
//         );
// }

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent), audioSink(nullptr), audioDevice(nullptr), frameSize(960)
{
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);

    if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format)) {
        qFatal("Audio format not supported");
    }

    audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), format, this);

    int status;
    opusDecoder = opus_decoder_create(48000, 1, &status);
    if (status != OPUS_OK)
        qFatal("Failed to create Opus decoder: %s", opus_strerror(status));

    audioDevice = audioSink->start();
    if (!audioDevice)
        qWarning() << "AudioOutput: Failed to start audio device";

    connect(this, &AudioOutput::newPacket, this, &AudioOutput::play);
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
    qDebug() << "AudioOutput(***): New data with size " << data.size() << " added to the queue.";

    emit newPacketGenerated();

    play();
}
