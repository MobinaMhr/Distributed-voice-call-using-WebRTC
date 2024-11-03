### Project
#### main Implementation
This is the entrly point of the application. It initializes the Qt application adn sets up the main window(UI).
### Audio
#### AudioInput Implementation

```c
signals:
    void bufferIsReady(const QByteArray &buffer);
```

```c
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
```

```c
AudioInput::~AudioInput() {
    if (audioSource)
        delete audioSource;
    if (encoder)
        opus_encoder_destroy(encoder);
}
```

```c
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
```

```c
void AudioInput::stop()
{
    if (audioSource)
        audioSource->stop();
}
```

```c
qint64 AudioInput::readData(char *data, qint64 maxLen)
{
    Q_UNUSED(data);
    Q_UNUSED(maxLen);
    return 0;
}
```

```c
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
        return -1;
    }

    QByteArray audioData(reinterpret_cast<const char*>(encodedData), encodedBytes);
    emit bufferIsReady(audioData);

    return len;
}
```

#### AudioOutput Implementation

```c
signals:
    void newPacketGenerated();
```

```c
AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent), audioSink(nullptr), audioDevice(nullptr),
    frameSize(960)
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
```

```c
AudioOutput::~AudioOutput()
{
    if (audioSink)
        delete audioSink;

    if (opusDecoder)
        opus_decoder_destroy(opusDecoder);
}
```

```c
void AudioOutput::addData(const QByteArray &data)
{
    QMutexLocker locker(&mutex);

    packetQueue.enqueue(data);

    emit newPacketGenerated();
}
```

```c
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
```

#### AudioProcessor Implementation

### Network
#### WebRTC Implementation

```c
WebRTC::WebRTC(QObject *parent)
    : QObject{parent},
    m_audio("Audio")
{
    connect(this, &WebRTC::gatheringComplited, [this] (const QString &peerID) {

        m_localDescription = descriptionToJson(peerID);
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);
        if (m_isOfferer)
            Q_EMIT this->offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerID, m_localDescription);
    });

    connect(this, &WebRTC::localCandidateGenerated, [this] (const QString &peerID, const QString &candidate,
                                                           const QString &mid){
        rtc::Candidate localCandidate(candidate.toStdString(), mid.toStdString());
        m_peerConnections[peerID]->localDescription()->addCandidate(localCandidate);
    });
}
```

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

```c
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    rtc::Description::Type type = (m_isOfferer) ? rtc::Description::Type::Answer
                                                : rtc::Description::Type::Offer;
    rtc::Description remoteDescription(sdp.toStdString(), type);

    m_peerConnections[peerID]->setRemoteDescription(remoteDescription);
}
```

```c
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    rtc::Candidate remoteCandidate(candidate.toStdString(), sdpMid.toStdString());
    m_peerConnections[peerID]->addRemoteCandidate(remoteCandidate);
}
```

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

### Src
#### CallManager Implementation

Here is an overveiw of constructor, in which we 

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

QJsonArray CallManager::getCandidatesQJsonArr(std::vector<rtc::Candidate> candidates) {
    QJsonArray candidateArray;

    for (const auto& candidate : candidates) {
        candidateArray.append(QString::fromStdString(candidate.candidate()));
    }

    return candidateArray;
}

```

```c
void CallManager::createSocket()
{
    const QUrl url(QStringLiteral("ws://165.232.44.143:3000"));
    socket = new Socket(url);
    socket->connectToServer();
    connect(socket, &Socket::messageReceived, this, &CallManager::handleIncomingSocketMessage, Qt::AutoConnection);
}

void CallManager::createWebrtc()
{
    webrtcPeerId = "1";
    webrtc = new WebRTC(this);
    webrtc->init(webrtcPeerId, false);
    handleWebrtcConncetions();
}
```

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

```c
void CallManager::startCall()
{
    callStartedComeOn = true;
    webrtc->setIsOfferer(true);
    webrtc->addPeer(webrtcPeerId);
}

void CallManager::endCall()
{
    callStartedComeOn = false;
    webrtc->closePeerConnection(webrtcPeerId);
}

void CallManager::registerUser()
{
    QString jsonReq;
    jsonReq = createJsonRequest({"reqType", "user"}, {"register", m_userName});
    socket->sendMessage(jsonReq);
}
```

```c
void CallManager::handleNewCandidate(const QJsonObject &candidate)
{
    webrtc->setRemoteCandidate(webrtcPeerId, candidate.value("Candidate").toString(),
                               candidate.value("mid").toString());
}
```

```c
void CallManager::handleSingalingOffer(const QJsonObject &offer)
{
    setCallerId( offer.value("user").toString());
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

```c
void CallManager::handleSingalingAnswer(const QJsonObject &answer)
{
    this->setCallerId(answer.value("user").toString());
    webrtc->setRemoteDescription(webrtcPeerId, answer.value("sdp").toString());
    webrtc->setRemoteCandidate(webrtcPeerId, answer.value("candidate").toString(), answer.value("mid").toString());
}
```

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

```c
```
