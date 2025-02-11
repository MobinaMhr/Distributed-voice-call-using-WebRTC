### main Implementation
This is the entrly point of the application. It initializes the Qt application adn sets up the main window(UI).
- `QGuiApplication app(argc, argv)` Initializes the Qt application
- `QQmlApplicationEngine engine` Sets up the QML application engine
- `CallManager callManager` Creates an instance of CallManager
- `const QUrl url0(QStringLiteral("ws://localhost:3000"))` Defines the WebSocket server URL
- `Socket socket(url0)` Creates a new Socket object with the specified URL
- `socket.connectToServer()` Connects to the WebSocket server
- `QString message = ...` Prepares a message
- `socket.sendMessage(message)` Sends the message to the server
- `engine.rootContext()->setContextProperty("callManager", &callManager)` Exposes the callManager instance to the QML context
- `const QUrl url(...)` Specifies the path to the QML file
- Connects the objectCreated signal to a lambda function that checks if the QML file loaded successfully
- `engine.load(url)` Loads the QML file
- `return app.exec();` Starts the Qt event loop, running the application

### Audio
#### AudioInput Implementation

This class leverages the `QIODevice` framework for reading and writing data.

The AudioInput constructor initializes the audio input device.
```c
QMediaDevices mediaDevices;
QAudioDevice inputDevice;
inputDevice = mediaDevices.defaultAudioInput();
if (!inputDevice.isFormatSupported(format)) {
    qFatal("Audio format not supported");
}

audioSource = new QAudioSource(inputDevice, format, this);
```
It first initializes the pointer to an object of AudioProcessor class to handle encoding raw audio.
```c
if (!processor->initializeEncoder()) {
    qWarning() << "AudioInput: Audio processor is not initialized!";
    return;
}
```
it also uses the static method of `AudioProcessor` to create a format for audio in `format` attribute.
```c
QAudioFormat format = AudioProcessor::createAudioFormat();
```

The desctructore deletes processor and audioSource pointers if they exist.

The `start` method of this class checks if the audioSource exists, attempts to open the device for reading and writing with `QIODevice::ReadWrite`, and Starts the `audioSource` with the current instance (`this`) as the device to which it will write audio data.

After all it checks if the audioSource is in `QAudio::ActiveState`.


The `stop` method halts the audio input process.
The method first checks if audioSource is initialized, and then calls audioSource->stop(), which stops the audio data capture.

The `readData` method is designed to read audio data from the device. Since the AudioInput class is designed to handle audio capture and encoding, `readData` remains unused in this implementation. It uses the `Q_UNUSED` macro for both `data` and `maxLen` parameters, indicating that these parameters are intentionally unused within the function. This method simply returns 0.

The `writeData` responsible for handling and encoding raw audio data, then emitting a signal when the buffer is ready.

It first defines a constant `maxPacketSize` which represents the maximum size of the encoded data packet.
After that a declaration of a buffer named `encodedData` to hold the encoded audio data.
It declares a buffer named `encodedData` to hold the encoded audio data.
The encoding Audio Data process starts as:
1. Calls `processor->encodeAudio` to encode the raw audio data. It takes the input data, frame size (half of the length since it's 16-bit), and the output buffer with the max packet size.
2. Checks if the encoding process is successful. If it fails, returns -1.

After all it creates a QByteArray object named `audioData` from the encoded data.
At the end the `bufferIsReady` signal is emited with the encoded audioData, and returns the length of the data written.

#### AudioOutput Implementation
This class leverages the `QObject` framework for audio data management and playback. 

The constructor initializes the audio output device and sets up the necessary components for audio playback.
It sets the audio format using static method of `AudioProcessor`.
```c
QAudioFormat format = AudioProcessor::createAudioFormat();
```

It immidiately checks if the format is supported by the `defaultAudioOutput`, which is the speacker of device.
```c
if (!QMediaDevices::defaultAudioOutput().isFormatSupported(format))
  qFatal("Audio format not supported");
```

The named `processor` object initializes the decoder.
```c
if (!processor->initializeDecoder()) {
  qFatal("Failed to initialize Opus decoder");
}
```

The `audioSink` is responsible for audio playback and writes the audio to the output device.
```c
audioSink = new QAudioSink(QMediaDevices::defaultAudioOutput(), format, this);
```

The `start()` method of `audioSink` is called to begin audio playback. It returns a pointer to a QIODevice, which is assigned to audioDevice. This `QIODevice` represents the device through which audio data will be written for playback.
```c
audioDevice = audioSink->start();
if (!audioDevice)
  qWarning() << "AudioOutput: Failed to start audio device";
```

When the `newPacketGenerated` signal is emitted, the `play` private slot is automatically invoked. This ensures that as soon as new audio data is ready, it gets processed and played back immediately.
```c
connect(this, &AudioOutput::newPacketGenerated, this, &AudioOutput::play);
```

The desctructore deletes processor and audioSink pointers if they exist.

The `addData()` method ensures that new audio data is safely added to the processing queue and immediately signals that data is ready to be processed for playback.
The `mutex lock` ensures that with the congestion of packets only one packet at a time can access this function. 
After queueing the packet we inform that a packet has arrived by emiting `newPacketGenerated` signal.

```c
QMutexLocker locker(&mutex);
packetQueue.enqueue(data);
emit newPacketGenerated();
```

In the `play()` method, after checking that we have at least one packet to play, we get the packet from queue.
```c
if (packetQueue.isEmpty()) {
  qWarning() << "AudioOutput: No available data to play.";
  return;
}

QByteArray packet = packetQueue.dequeue();
```

We decode the packet using `processor->decodeAudio()` method and after a successful decoding, the incomming audio is written into the output device.

```c
void AudioOutput::play()
{
    int decodingResult = processor->decodeAudio(&packet, outputBuff, frameSize);
    if (decodingResult < 0) {
        qWarning() << "AudioOutput: Decoder process failed.";
        return;
    }

    audioDevice->write(reinterpret_cast<const char*>(outputBuff), decodingResult * sizeof(opus_int16));
}
```

A challange in this projet was that we thought the `opus_int16` and other configs of encoding and decoding.
The type should be the buffer type, but after reading the documentaion of opus, we set the true type and config for them.

#### AudioProcessor Implementation

This class handles the processing actions on audio. It has a static method called `createAudioFormat`.
It is sets up an audio format with a sample rate, channel count, and a sample format which is of QAudioFormat::Int16 

This static method and is called in both AudioInput and AudioOutput classes to get a consistent format for auidio.

- The sample rate is 48KHz which is in range of what human ears can hear. 
- We've used a single channel, but you can use 2 channels if you have a stereo speaker.

```c
QAudioFormat AudioProcessor::createAudioFormat() {
    QAudioFormat format;
    format.setSampleRate(48000);
    format.setChannelCount(1);
    format.setSampleFormat(QAudioFormat::Int16);
    return format;
}
```

The encoder and decoder object should be initialized.
Since Opus is a stateful codec, the encoding and decoding process starts with creating an encoder state.

```c
bool AudioProcessor::initializeDecoder() {
    int status;
    decoder = opus_decoder_create(48000, 1, &status);
    if (status != OPUS_OK) {
        qWarning() << "AudioProcessor: Failed to create Opus decoder:" << opus_strerror(status);
        return false;
    }
    return true;
}

bool AudioProcessor::initializeEncoder() {
    int status;
    encoder = opus_encoder_create(48000, 1, OPUS_APPLICATION_VOIP, &status);
    if (status != OPUS_OK) {
        qWarning() << "AudioProcessor: Failed to create Opus encoder";
        return false;
    }
    return true;
}
```

They return boolean types to make sure the opus objects are initialized successfully.

In the destructor, we delete destroy `encoder` and `decoder` if they exist. This is done with built-in methods in ***opus** library.

- The `opus_decoder_destroy` Free an OpusDecoder allocated by `opus_decoder_create`. 
- The `opus_encoder_destroy` Free an OpusEncoder allocated by `opus_encoder_create`. 

```c
if (decoder) {
    opus_decoder_destroy(decoder);
    decoder = nullptr;
}

if (encoder) {
    opus_encoder_destroy(encoder);
    encoder = nullptr;
}
```

The `encodeAudio` method is called by AudioInput class and encodes raw audio data using the Opus codec.
This method takes the following parameters:

`input` is a pointer to the raw audio data to be encoded. `frameSize` is the number of audio samples in each frame. `output` is a buffer to hold the encoded data. `maxPacketSize` is the maximum size of the encoded packet.

The method first checks if the Opus encoder is initialized. If not, it logs a warning and returns -1 indicating an error. Otherwise, it calls the `opus_encode` function to perform the encoding, converting the input audio samples into the compressed Opus format, and returns the number of bytes written to the output buffer.

```c
int AudioProcessor::encodeAudio(const char *input, int frameSize, unsigned char* output, int maxPacketSize) {
    if (!encoder) {
        qWarning() << "AudioProcessor: Opus encoder is not initialized!";
        return -1;
    }

    return opus_encode(encoder, reinterpret_cast<const opus_int16*>(input),
                       frameSize, output, maxPacketSize);
}
```

The `decodeAudio` method is called by AudioOutput class and decodes encoded audio data using the Opus codec. 

`data` is a pointer to a QByteArray containing the encoded audio data. `output` is a buffer to hold the decoded audio samples. `frameSize` is the number of audio samples to decode in each frame.

The method first checks if the Opus decoder is initialized. If not, it logs a warning and returns -1 to indicate an error. It also verifies the size of the encoded packet. If the packet size is less than 1, it logs a warning and returns -1.

If all checks pass, it calls the `opus_decode` function to decode the audio data, converting the encoded input into raw audio samples and storing them in the output buffer. The function returns the number of samples decoded.

```c
int AudioProcessor::decodeAudio(const QByteArray *data, opus_int16 *output, int frameSize) {
    if (!decoder) {
        qWarning() << "AudioProcessor: Opus decoder is not initialized!";
        return -1;
    }

    int packetSize = data->size();
    if (packetSize < 1) {
        qWarning() << "AudioProcessor: Invalid encoded packet size:" << packetSize;
        return -1;
    }

    return opus_decode(decoder, reinterpret_cast<const unsigned char *>(data->data()),
                       packetSize, output, frameSize, 0);
}
```

### Network
#### WebRTC Implementation

##### Public Methods 

WebRTC, short for **Web Real-Time Communication**, is a technology that allows peer-to-peer audio, video, and data sharing directly between web browsers and mobile applications. It's an open-source project that provides web applications and sites with real-time communication capabilities via simple JavaScript APIs.

In the constructor of it, we code for some signal handlings.
The `gatheringComplited` Signal connects the gatheringComplited signal to a lambda function that handles the completion of ICE candidate gathering. It generates the local description and emits the localDescriptionGenerated signal. And depending on the role (offerer or answerer), emits either the offerIsReady or answerIsReady signal.
```c
connect(this, &WebRTC::gatheringComplited, [this] (const QString &peerID) {

    m_localDescription = descriptionToJson(peerID);
    Q_EMIT localDescriptionGenerated(peerID, m_localDescription);
    if (m_isOfferer)
        Q_EMIT this->offerIsReady(peerID, m_localDescription);
    else
        Q_EMIT this->answerIsReady(peerID, m_localDescription);
});
```

The `localCandidateGenerated` Signal connects the localCandidateGenerated signal to a lambda function that handles the generation of local ICE candidates. It converts the candidate data to an rtc::Candidate and adds it to the local description of the peer connection.
```c
connect(this, &WebRTC::localCandidateGenerated, [this] (const QString &peerID, const QString &candidate,
                                                       const QString &mid){
    rtc::Candidate localCandidate(candidate.toStdString(), mid.toStdString());
    m_peerConnections[peerID]->localDescription()->addCandidate(localCandidate);
});
```

In the `init()` method, we :
- Initializes the logging system with a debug level to capture detailed logs.
- We set some default values to some attributes.
- we build a config of ICE server configurations.
- We sets up an audio description for sending and receiving audio streams.
```c
void WebRTC::init(const QString &id, bool isOfferer)
{
    rtc::InitLogger(rtc::LogLevel::Debug);

    m_localId = id;
    m_isOfferer = isOfferer;

    m_config.iceServers.clear();
    m_config = rtc::Configuration();
    m_config.iceServers.emplace_back("stun:165.232.44.143:3478");
    rtc::IceServer turnServer("turn:165.232.44.143", "3479", "myturn", "mewmew");
    m_config.iceServers.emplace_back(turnServer);
    // m_config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    m_audio = rtc::Description::Audio("audio", rtc::Description::Direction::SendRecv);
    m_audio.addOpusCodec(payloadType());
    m_audio.setBitrate(bitRate());
    m_audio.addSSRC(ssrc(), "audio-send");
}
```

In the `addPeer()` method, sets up a new peer connection and configures all necessary event handlers to manage the connection's lifecycle, including handling local descriptions, ICE candidates, state changes, and gathering state.

- **Peer Connection Initialization**
  - Creates a new `PeerConnection` object using the previously configured `m_config`.
  - Stores the peer connection in the `m_peerConnections` map using `peerId` as the key.
- **Local Description Handler**:
  - Sets up a handler to emit the localDescriptionGenerated signal when a local description is created.
- **Local Candidate Handler**:
  - Sets up a handler to emit the localCandidateGenerated signal when a local ICE candidate is generated.
- **State Change Handler**:
  - Monitors the state of the peer connection and logs changes or emits signals as appropriate.
- **Gathering State Change Handler**:
  - Monitors the ICE candidate gathering state and emits the gatheringComplited signal when gathering is complete.
- **Add Audio Track and Set Local Description**:
  - Adds an audio track to the peer connection and sets the local description.
```c
void WebRTC::addPeer(const QString &peerId)
{
    auto pc = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections[peerId] = pc;

    pc->onLocalDescription([this, peerId](const rtc::Description &description) {
        Q_EMIT localDescriptionGenerated(peerId, QString::fromStdString(description));
    });

    pc->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        Q_EMIT localCandidateGenerated(peerId, QString::fromStdString(candidate.candidate()),
                                       QString::fromStdString(candidate.mid()));
    });

    pc->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        switch(state) {
        case rtc::PeerConnection::State::New:
            qDebug() << "WEBRTC(___)" << "Peer connection state: New";
            break;
        case rtc::PeerConnection::State::Connecting:
            qDebug() << "WEBRTC(___)" << "Peer connection state: Connecting" ;
            break;
        case rtc::PeerConnection::State::Connected:
            Q_EMIT connectionStablished();
            qDebug() << "WEBRTC(___)" << "Peer connection state: Connected" ;
            break;
        case rtc::PeerConnection::State::Disconnected:
            qDebug() << "WEBRTC(___)" << "Peer connection state: Disconnected" ;
            break;
        case rtc::PeerConnection::State::Failed:
            qDebug() << "WEBRTC(___)" << "Peer connection state: Failed" ;
            break;
        case rtc::PeerConnection::State::Closed:
            Q_EMIT connectionClosed();
            qDebug() << "WEBRTC(___)" << "Peer connection state: Closed" ;
            break;
        }
    });

    pc->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::New)
            qDebug() << "WEBRTC(___)" << "state is New";
        if (state == rtc::PeerConnection::GatheringState::InProgress)
            qDebug() << "WEBRTC(___)" << "state is inProgress";
        if (state == rtc::PeerConnection::GatheringState::Complete){
            qDebug() << "WEBRTC(___)" << "state is complete";
            Q_EMIT gatheringComplited(peerId);
        }
    });

    addAudioTrack(peerId, QString::fromStdString("Hasti:("));
    pc->setLocalDescription();
}
```

A challange in coding was unnessesary setLocalDescription in constructor of htis class. It was the cause of faillure. We checked the webrtc procedure and found it out.

The `generateOfferSDP` and `generateAnswerSDP` methods are responsible for generating the Session Description Protocol (SDP) offers and answers in a WebRTC connection.
Thye are crucial for setting up the SDP offers and answers in the WebRTC handshake process. By calling setLocalDescription with the appropriate type (Offer or Answer), they ensure that the peer connection is correctly configured for initiating or responding to a connection request.
```c
void WebRTC::generateOfferSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
}

void WebRTC::generateAnswerSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
}
```

The `addAudioTrack` method adds an audio track to a peer connection and sets up a handler to process and emit incoming audio packets.
- Retrieves the peer connection associated with the specified peerId.
- Adds the pre-configured audio description (m_audio) to the peer connection and creates a track.
- Stores the audio track in the m_peerTracks map using peerId as the key.
- Sets up a handler to process incoming messages on the audio track:
  - Converts the incoming binary message to a QByteArray and removes the RTP header if present.
  - Emits the incommingPacket signal with the processed data.
```c
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    auto pc = m_peerConnections[peerId];

    auto track = pc->addTrack(m_audio);
    m_peerTracks[peerId] = track;

    track->onMessage([this, peerId](rtc::binary message){
        QByteArray data = QByteArray(reinterpret_cast<const char*>(message.data()), message.size());
        if (data.size())
            data.remove(0, sizeof(RtpHeader));
        Q_EMIT incommingPacket(peerId, data, data.size());
    }, nullptr);
}
```

A challange in project was this:
We forgot to remove the RTP header and the heared voice was incomprehensible and noisy for both of use.

The `sendTrack` method handles the process of sending an audio track to a peer connection. It constructs an RTP packet by adding a properly formatted header and the audio data. It then sends the packet over the WebRTC connection, ensuring the data is transmitted to the intended peer.
- RTP Header Construction:
- first: Set to 0x80, indicating the RTP version.
  - `marker`: Set to 0, not marking the end of a frame.
  - `payloadType`: Set using payloadType(), indicating the codec type.
  - `sequenceNumber`: Incremented and converted to big endian using qToBigEndian.
  - `timestamp`: Current timestamp, converted to big endian using qToBigEndian.
  - `ssrc`: Synchronization source identifier, converted to big endian using qToBigEndian.
- RTP Packet Construction:
  - Append Header: The header is appended to the RTP packet.
  - Append Buffer: The buffer containing the audio data is appended to the RTP packet.
- Sending Data:
  - The RTP packet data is converted to rtc::byte and sent to the peer track.
  - Error Handling: If an error occurs during sending, it is caught and logged.
```c
void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    RtpHeader header;
    header.first = 0x80;
    header.marker = 0;
    header.payloadType = payloadType();
    header.sequenceNumber = qToBigEndian(m_sequenceNumber++);
    header.timestamp = qToBigEndian(getCurrentTimestamp());
    header.ssrc = qToBigEndian(ssrc());

    QByteArray rtpPacket;
    rtpPacket.append(reinterpret_cast<char*>(&header), sizeof(RtpHeader));
    rtpPacket.append(buffer);

    const rtc::byte *data = reinterpret_cast<const rtc::byte *>(rtpPacket.constData());
    try {
        m_peerTracks[peerId]->send(data, rtpPacket.size());
    } catch (const std::exception &e) {
        qWarning() << "Error sending track: " << e.what();
    }
}
```

##### Public Slots 

The `setRemoteDescription` method configures the remote description for a WebRTC peer connection, allowing the session to be properly established with the other peer. 
It is used for establishing the WebRTC session by setting the remote description.
It ensures that the connection is properly configured for communication with the other peer. The description type is dynamically set based on the instance's role (offerer or answerer).
```c
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    rtc::Description::Type type = (m_isOfferer) ? rtc::Description::Type::Answer
                                                : rtc::Description::Type::Offer;
    rtc::Description remoteDescription(sdp.toStdString(), type);

    m_peerConnections[peerID]->setRemoteDescription(remoteDescription);
}
```

This `setRemoteCandidate` method sets a remote ICE candidate for a specific peer connection.
It is crucial for establishing the peer-to-peer connection in WebRTC.

It handles the ICE negotiation process in WebRTC. By adding the remote candidate, it ensures that the peer connection can establish the optimal network path for real-time communication. 
```c
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    rtc::Candidate remoteCandidate(candidate.toStdString(), sdpMid.toStdString());
    m_peerConnections[peerID]->addRemoteCandidate(remoteCandidate);
}
```

##### Private Methods 

The `descriptionToJson()` method converts the local description of a peer connection into a JSON format string, in order to share it and do process on it.
```c
QString WebRTC::descriptionToJson(const QString &peerID)
{
    auto description = m_peerConnections[peerID]->localDescription();

    QJsonObject jsonMessage{
        {"type", QString::fromStdString(description->typeString())},
        {"sdp", QString::fromStdString(description.value())}
    };

    QJsonDocument doc(jsonMessage);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}
```

The `closePeerConnection` method handles the closing of a peer connection in a WebRTC session, ensuring that resources are properly released and connections are terminated.
- First checks if there is an active peer connection for the specified peerId
- If the peer tracks contain an entry for peerId and the instance is the offerer, it closes the audio track.
- It then closes the peer connection if the instance is the offerer and removes it from the m_peerConnections map.
```c
void WebRTC::closePeerConnection(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {

        if (m_peerTracks.contains(peerId)) {
            if (m_isOfferer)
                m_peerTracks[peerId]->close();
            m_peerTracks.remove(peerId);
        }

        auto pc = m_peerConnections[peerId];
        if (m_isOfferer)
            pc->close();
        m_peerConnections.remove(peerId);

        qDebug() << "WEBRTC(___)" << "Peer connection closed for peer:" << peerId;

    } else {
        qWarning() << "WEBRTC(___)" << "No peer connection found for peer:" << peerId;
    }
}
```

This method retrieves all ICE candidates for a specified peer connection.
```c
std::vector<rtc::Candidate> WebRTC::getCandidates(const QString &peerID)
{
    return m_peerConnections[peerID]->localDescription()->candidates();
}
```

##### Getters and Setters
There are some getters and setters for this attributes. They simply return the value, sets the new value of the attribute and emits corresponding signal.

#### Socket Implementation

In this class, `QWebSocket::connected`, ``, and `` signals are used and connected to 

In the constructore of this class, we bind all th signals built into the `QWebSocket` with private slots.

```c
connect(m_webSocket, &QWebSocket::connected, this, &Socket::onConnected);
connect(m_webSocket, &QWebSocket::disconnected, this, &Socket::onDisconnected);
connect(m_webSocket, &QWebSocket::textMessageReceived, this, &Socket::onMessageReceived);
```

- `QWebSocket::connected` informs the successful connection to the websocket.</br>
-  `QWebSocket::disconnected` informs losing the connection to the websocket.</br>
- `QWebSocket::textMessageReceived` informs arrival of new message to the peer.</br>

The `onConnected`, `onDisconnected`, and `onMessageReceived` private slots emit the corresponding signals.

The first one is called when the WebSocket connection is successfully established. It serves as a notification mechanism to other components in the application that rely on the connection status.

The second one is invoked when the WebSocket connection is closed. It notifies other components of the disconnection, allowing the application to handle the loss of connection appropriately.

The third one is called whenever a new message is received through the WebSocket. It enables the application to process incoming messages and react accordingly.

The destructor of this class deletes the created `m_webSocket`.

The `connectToServer` method initiates a WebSocket connection to the specified server URL. It uses the open function of the m_webSocket object, which represents the WebSocket client.

```c
m_webSocket->open(m_url);
```
The `disconnectFromServer` method gracefully closes an active WebSocket connection. It leverages the close function of the m_webSocket object to terminate the connection.

```c
m_webSocket->close();
```

The `sendMessage` method allows the client to send a text message via the WebSocket. Before sending, it checks if the WebSocket connection is valid using isValid.

```c
if (m_webSocket->isValid())
    m_webSocket->sendTextMessage(message);
```

The `isConnected` method checks the current state of the WebSocket connection. It returns true if the WebSocket's state is ConnectedState, indicating an active connection, and false otherwise.

```c
return m_webSocket->state() == QAbstractSocket::ConnectedState;
```

### backend
#### CallManager Implementation

The CallManager constructor initializes the call manager, sets up audio input and output, and handles WebRTC and socket creation.

At the end it connects the `bufferIsReady` signal from audioInput to a lambda function that sends the track data via WebRTC. 
It ensures the audio buffer is sent over the WebRTC connection whenever new audio data is ready.

```c
CallManager::CallManager(QObject *parent)
    : QObject{parent},
    m_ipAddress("172.16.142.176"),
    m_iceCandidate("172.16.142.176")
{
    audioInput = new AudioInput(this);
    audioOutput = new AudioOutput(this);
    callStartedComeOn = false;
    createWebrtc();

    createSocket();

    connect(audioInput, &AudioInput::bufferIsReady, this, [this](const QByteArray &buffer){
        webrtc->sendTrack(webrtcPeerId, buffer);
    }, Qt::AutoConnection);
}
```

The destructore deletes the pointers corresponding to webrtc and socket, if they exist.

```c
CallManager::~CallManager()
{
    if (webrtc)
        delete webrtc;

    if (socket) {
        socket->disconnectFromServer();
        delete socket;
    }
}
```
This method establishes various connections between the CallManager and the WebRTC instance to handle WebRTC signaling and audio data transmission.
- Offer and Answer Handling:
    - Connects to the offerIsReady and answerIsReady signals from the WebRTC instance.
    - When either signal is emitted, it constructs a JSON message with the offer or answer details and sends it via the WebSocket.

- Incoming Packet Handling:
    - Connects to the incommingPacket signal to process incoming audio data and add it to the audio output.

- Connection State Handling:
    - Connects to the connectionStablished signal to start the audio input when the connection is established.
    - Connects to the connectionClosed signal to stop the audio input when the connection is closed.

- Candidate Generation Handling:
    - Connects to the localCandidateGenerated signal to handle new ICE - - candidates by sending them via the WebSocket.
```c
void CallManager::handleWebrtcConncetions()
{
    connect(webrtc, &WebRTC::offerIsReady, this, [this](const QString &peerId, const QString& description) {
        if (callStartedComeOn){
            QString updatedJsonString = getCompletedJson(description, "offer");
            this->socket->sendMessage(updatedJsonString);
        }
    }, Qt::AutoConnection);

    connect(webrtc, &WebRTC::answerIsReady, this,[this](const QString &peerId, const QString& description) {
        QString updatedJsonString = getCompletedJson(description, "answer");
        this->socket->sendMessage(updatedJsonString);
    }, Qt::AutoConnection);

    connect(webrtc, &WebRTC::incommingPacket, this, [this](const QString &peerId, const QByteArray &data, qint64 len) {
        audioOutput->addData(data);
    }, Qt::AutoConnection);

    connect(webrtc, &WebRTC::connectionStablished, this, [this] (){
        audioInput->start();
    }, Qt::AutoConnection);

    connect(webrtc, &WebRTC::connectionClosed, this, [this](){
        this->audioInput->stop();
    }, Qt::AutoConnection);

    connect(webrtc, &WebRTC::localCandidateGenerated, this, [this](const QString &peerID,
                                                    const QString &candidate, const QString &mid){
        if (callStartedComeOn){
            QString jsonReq = createJsonRequest({"reqType", "target", "candidate", "mid"},
                                                {"candidate", m_callerId, candidate, mid});

        }
    }, Qt::AutoConnection);
}
```

This method constructs a JSON message from the WebRTC session description, including the type, user, target, and ICE candidates.
```c
QString CallManager::getCompletedJson(const QString& description, const QString type)
{
    QJsonDocument doc = QJsonDocument::fromJson(description.toUtf8());

    if (!doc.isObject())
        qWarning() << "Invalid JSON format";

    QJsonObject jsonObj = doc.object();    

    jsonObj["reqType"] = type;
    jsonObj["user"] = m_userName;
    jsonObj["target"] = m_callerId;
    jsonObj["candidate"] = getCandidatesQJsonArr(webrtc->getCandidates(webrtcPeerId));
    jsonObj["mid"] = webrtc->getMid(webrtcPeerId);

    return QJsonDocument(jsonObj).toJson(QJsonDocument::Compact);
}
```

This method converts a vector of rtc::Candidate objects to a QJsonArray for inclusion in JSON messages.
```c
QJsonArray CallManager::getCandidatesQJsonArr(std::vector<rtc::Candidate> candidates) {
    QJsonArray candidateArray;

    for (const auto& candidate : candidates) {
        candidateArray.append(QString::fromStdString(candidate.candidate()));
    }

    return candidateArray;
}
```

The first line of `createSocket()` method defines our WebSocket server URL.
After that, it creates a new Socket object with the specified URL.
Then initiates the connection to the WebSocket server.

The signal-Slot Connection connects the `messageReceived` signal from the Socket to the `handleIncomingSocketMessage` slot in CallManager, ensuring incoming messages are properly handled.

```c
void CallManager::createSocket()
{
    const QUrl url(QStringLiteral("ws://165.232.44.143:3000"));
    socket = new Socket(url);
    socket->connectToServer();
    connect(socket, &Socket::messageReceived, this, &CallManager::handleIncomingSocketMessage, Qt::AutoConnection);
}
```

The `createWebrtc()` method initializes the WebRTC components.
It sets the WebRTC peer ID, creates a new WebRTC object.
Tehn it initializes the WebRTC object with the peer ID and a boolean flag (it sets the `isOffere` as false).

```c
void CallManager::createWebrtc()
{
    webrtcPeerId = "1";
    webrtc = new WebRTC(this);
    webrtc->init(webrtcPeerId, false);
    handleWebrtcConncetions();
}
```

The setter methods simply set new value to attributes and emits the corresponding signal. 

```c
void CallManager::setCallerId(const QString &callerId)
{
    if (m_callerId != callerId) {
        m_callerId = callerId;
        emit callerIdChanged();
    }
}

void CallManager::setUserName(const QString &userName)
{
    if (m_userName != userName) {
        m_userName = userName;
        emit userNameChanged();
    }
}
```


If a peer wants to start a call with another one, the `startCall()` method is called and sets some boolean values and adds the `webrtcPeerId` as a peer in the protocol.
```c
void CallManager::startCall()
{
    callStartedComeOn = true;
    webrtc->setIsOfferer(true);
    webrtc->addPeer(webrtcPeerId);
}
```

If a peer wants to end a call with another one, the `endCall()` method is called and a boolean value is set to default value (false) and the created peer connection as `webrtcPeerId` si closed.
```c
void CallManager::endCall()
{
    callStartedComeOn = false;
    webrtc->closePeerConnection(webrtcPeerId);
}
```

When a user tries to register in, the `registerUser()` method is called and a request in json format is send as a message via socket to the server.
```c
void CallManager::registerUser()
{
    QString jsonReq;
    jsonReq = createJsonRequest({"reqType", "user"}, {"register", m_userName});
    socket->sendMessage(jsonReq);
}
```

This method handles the incomming message from server via socket. it calls other handler methods to handle `offer`, `answer`, `registration` messages.
```c
void CallManager::handleIncomingSocketMessage(const QString &message)
{
    QJsonObject jsonDoc;
    jsonDoc = QJsonDocument::fromJson(message.toUtf8()).object();

    if (jsonDoc.value("type").toString() == "offer")
        handleSingalingOffer(jsonDoc);
    else if (jsonDoc.value("type").toString() == "answer")
        handleSingalingAnswer(jsonDoc);
    else
        handleNewCandidate(jsonDoc);
}
```

This method is not called in project. However, it serves the purpose of handling scenarios where a new ICE candidate is generated post-connection setup. In such cases, the signaling server notifies us, allowing the method to set the remote candidate accordingly.
```c
void CallManager::handleNewCandidate(const QJsonObject &candidate)
{
    webrtc->setRemoteCandidate(webrtcPeerId, candidate.value("Candidate").toString(),
                               candidate.value("mid").toString());
}
```

When an offer is received for this peer, the webrtc inits it. Since this peer is answerer, its `isOfferer` should be set to false. We sets its caller id for display. We call `addPeer` on the `webrtcPeerId`. extract all its ICE candidates and add them sequentially.
```c
void CallManager::handleSingalingOffer(const QJsonObject &offer)
{
    setCallerId(offer.value("user").toString());
    webrtc->init(webrtcPeerId, false);
    webrtc->addPeer(webrtcPeerId);
    webrtc->setRemoteDescription(webrtcPeerId, offer.value("sdp").toString());
    std::vector<rtc::Candidate> candidates = extractCandidates(offer.value("sdp").toString());
    for (int i = 0 ; i < candidates.size(); i++)
        webrtc->setRemoteCandidate(webrtcPeerId, QString::fromStdString(candidates[i].candidate()),
                                   offer.value("mid").toString());
}

std::vector<rtc::Candidate> CallManager::extractCandidates(const QString &sdp)
{
    rtc::Description remoteDescription(sdp.toStdString());
    return remoteDescription.extractCandidates();
}
```

When an answer is received for this peer, this method sets remote description and candidates.
```c
void CallManager::handleSingalingAnswer(const QJsonObject &answer)
{
    this->setCallerId(answer.value("user").toString());
    webrtc->setRemoteDescription(webrtcPeerId, answer.value("sdp").toString());
    webrtc->setRemoteCandidate(webrtcPeerId, answer.value("candidate").toString(), answer.value("mid").toString());
}
```

The `createJsonRequest` method constructs a JSON-formatted string from two vectors of keys and values.
```c
QString CallManager::createJsonRequest(const std::vector<QString> &keys, const std::vector<QString> &values) {
    int size = keys.size();
    QJsonObject jsonObject;

    for (int i = 0; i < size; i++) {
        QString key = keys[i];
        QString value = values[i];
        jsonObject[key] = value;
    }

    QJsonDocument jsonDoc(jsonObject);
    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    return jsonString;
}
```

### UI
#### QML main Implementation

In this file, we've coded for the UI part of the project, the popuop window mentioned in [How to Run](https://github.com/MobinaMhr/Computer-Networks-Course-S2025/blob/main/CA1/README.md), shows the `Ip`, `IceCandidate`, `Id`, `CallerId` which are in `ColumnLayout`.
At the bellow of them, there are two `TextField`s for Your Id and Phone Number. Two `Button`s are bellow. 

- ColumnLayout
    It arranges its child items vertically in a column, one below the other.
- TextField
    These have some visual features as anchors and ... 
    The `onTextChanged` part, does all of what is done after writing the corresponding box of text. 
- Button
    these have some visual features as anchors, height, width and ...
    The `onClicket` part, does all of what is done after corresponding buttons are clicked by user.
  
```c
```
