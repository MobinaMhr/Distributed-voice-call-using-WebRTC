//#include "audioinput.h"
//
//AudioInput::AudioInput(QObject *parent)
//    : QIODevice(parent), audioSource(nullptr), opusEncoder(nullptr), dataChannel(nullptr) {
//    // Set up audio format for capturing audio
//    QAudioFormat format;
//    format.setSampleRate(48000);       // Sample rate of 48kHz
//    format.setChannelCount(1);         // Mono audio
//    format.setSampleSize(16);          // 16-bit PCM
//    format.setCodec("audio/pcm");
//    format.setByteOrder(QAudioFormat::LittleEndian);
//    format.setSampleType(QAudioFormat::SignedInt);
//
//    // Initialize QAudioSource
//    audioSource = new QAudioSource(format, this);
//
//    // Initialize Opus encoder
//    int error;
//    opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &error);
//    if (error != OPUS_OK) {
//        qWarning() << "Failed to create Opus encoder:" << opus_strerror(error);
//    }
//}
//
//AudioInput::~AudioInput() {
//    stop();  // Ensure the audio source is stopped
//    opus_encoder_destroy(opusEncoder);  // Clean up Opus encoder
//}
//
//void AudioInput::start() {
//    // Start capturing audio
//    audioIODevice = audioSource->start();  // Start the audio input device
//    connect(audioIODevice, &QIODevice::readyRead, this, &AudioInput::handleAudioInput);  // Connect signal for handling audio
//    qDebug() << "Audio input started.";
//}
//
//void AudioInput::stop() {
//    // Stop capturing audio
//    audioSource->stop();  // Stop the audio input device
//    disconnect(audioIODevice, &QIODevice::readyRead, this, &AudioInput::handleAudioInput);  // Disconnect the signal
//    qDebug() << "Audio input stopped.";
//}
//
//qint64 AudioInput::writeData(const char *data, qint64 len) {
//    // Encode the audio data using Opus
//    unsigned char encodedData[4000];  // Buffer for encoded data
//    int encodedBytes = opus_encode(opusEncoder, reinterpret_cast<const opus_int16 *>(data), len / 2, encodedData, sizeof(encodedData));
//
//    if (encodedBytes < 0) {
//        qWarning() << "Opus encoding error:" << opus_strerror(encodedBytes);
//        return 0;
//    }
//
//    // Send the encoded data over the WebRTC data channel
//    if (dataChannel) {
//        // if (dataChannel->readyState() != rtc::DataChannelInterface::kOpen) {
//        //     qWarning() << "Data channel is not open";
//        //     return 0;
//        // }
//        size_t data_size = sizeof(encodedData) / sizeof(encodedData[0]);
//        dataChannel->send(encodedData, data_size);
//        return encodedBytes;  // Return the number of encoded bytes sent
//    }
//
//    return len;  // Return the number of bytes written
//}
//
//qint64 AudioInput::readData(char *data, qint64 maxlen) {
//    // Not needed for this case (audio input), can return 0
//    return 0;
//}
//
