#include "audioinput.h"

AudioInput::AudioInput(QObject *parent)
   : QIODevice(parent), audioSource(nullptr), opusEncoder(nullptr), dataChannel(nullptr) {

    // Set up audio format for capturing audio
   QAudioFormat format;
   format.setSampleRate(48000);       // Sample rate of 48kHz
   format.setChannelCount(1);         // Mono audio
   // format.setSampleSize(16);          // 16-bit PCM
   // format.setCodec("audio/pcm");
   // format.setByteOrder(QAudioFormat::LittleEndian);
   // format.setSampleType(QAudioFormat::Int16); //Int32

   // Initialize QAudioSource
   audioSource = new QAudioSource(format, this);

   // Initialize Opus encoder
   int error;
   opusEncoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_AUDIO, &error);
   if (error != OPUS_OK) {
       qWarning() << "Failed to create Opus encoder:" << opus_strerror(error);
   }
}

AudioInput::~AudioInput() {
   stop();
   opus_encoder_destroy(opusEncoder);
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

