#include "callmanager.h"

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

CallManager::~CallManager()
{
    if (webrtc)
        delete webrtc;

    if (socket) {
        socket->disconnectFromServer();
        delete socket;
    }
}

QString CallManager::ipAddress() const
{
    return m_ipAddress;
}

QString CallManager::iceCandidate() const
{
    return m_iceCandidate;
}

QString CallManager::callerId() const
{
    return m_callerId;
}

QString CallManager::userName() const
{
    return m_userName;
}

void CallManager::setIpAddress(const QString &ipAddress)
{
    if (m_ipAddress != ipAddress) {
        m_ipAddress = ipAddress;
        emit ipAddressChanged();
    }
}

void CallManager::setIceCandidate(const QString &iceCandidate)
{
    if (m_iceCandidate != iceCandidate) {
        m_iceCandidate = iceCandidate;
        emit iceCandidateChanged();
    }
}

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

void CallManager::handleNewCandidate(const QJsonObject &candidate)
{
    qDebug() << "\n\n\nnewCandidate recieved\n\n\n";
    webrtc->setRemoteCandidate(webrtcPeerId, candidate.value("Candidate").toString(),
                               candidate.value("mid").toString());
}

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

void CallManager::handleSingalingOffer(const QJsonObject &offer)
{
    m_callerId = offer.value("user").toString();
    webrtc->init(webrtcPeerId, false);
    webrtc->addPeer(webrtcPeerId);
    webrtc->setRemoteDescription(webrtcPeerId, offer.value("sdp").toString());
    std::vector<rtc::Candidate> candidates = extractCandidates(offer.value("sdp").toString());
    for (int i = 0 ; i < candidates.size(); i++)
        webrtc->setRemoteCandidate(webrtcPeerId, QString::fromStdString(candidates[i].candidate()),
                                   offer.value("mid").toString());
}

void CallManager::handleSingalingAnswer(const QJsonObject &answer)
{
    this->setCallerId(answer.value("user").toString());
    webrtc->setRemoteDescription(webrtcPeerId, answer.value("sdp").toString());
    webrtc->setRemoteCandidate(webrtcPeerId, answer.value("candidate").toString(), answer.value("mid").toString());
}

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

std::vector<rtc::Candidate> CallManager::extractCandidates(const QString &sdp)
{
    rtc::Description remoteDescription(sdp.toStdString());
    return remoteDescription.extractCandidates();
}
