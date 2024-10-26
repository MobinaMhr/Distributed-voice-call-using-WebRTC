// #include "audioinput.h"

// AudioInput::AudioInput(QObject *parent)
//     : QIODevice(parent), audioSource(nullptr), opusEncoder(nullptr) {
//     int sampleRate = 48000;
//     int channelCount = 1;

//     QMediaDevices mediaDevices;
//     QAudioDevice inputDevice;

//     QAudioFormat audioFormat;
//     audioFormat.setSampleRate(sampleRate);         // 48kHz
//     audioFormat.setChannelCount(channelCount);     // Mono audio
//     audioFormat.setSampleFormat(QAudioFormat::Int16);

//     inputDevice = mediaDevices.defaultAudioInput();
//     if (!inputDevice.isFormatSupported(audioFormat)) {
//         qDebug() << "AudioInput:: Audio format is not supported.";
//     }

//     audioSource = new QAudioSource(inputDevice, audioFormat, this);
//     if (!audioSource) {
//         qDebug() << "AudioInput:: Audio format is not supported.";
//     }

//     int error; // may be OPUS_APPLICATION_VOIP
//     opusEncoder = opus_encoder_create(sampleRate, channelCount, OPUS_APPLICATION_AUDIO, &error);
//     if (error != OPUS_OK) {
//         qDebug() << "AudioInput:: Failed to create Opus encoder:" << opus_strerror(error);
//     }
// }

// AudioInput::~AudioInput() {
//     if (opusEncoder) {
//         opus_encoder_destroy(opusEncoder);
//     }
//     if (audioSource) {
//         delete audioSource;
//     }
// }

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
