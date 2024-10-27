#include "callmanager.h"

CallManager::CallManager(QObject *parent)
    : QObject{parent},
    m_ipAddress("172.16.142.176"),
    m_iceCandidate("172.16.142.176")
{
    createWebRTC("1", false);


    const QUrl url(QStringLiteral("ws://localhost:3000"));

    socket = new Socket(url);
    socket->connectToServer();
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

void CallManager::setCallerId(const QString &callerId)
{
    if (m_callerId != callerId) {
        m_callerId = callerId;
        emit callerIdChanged();
    }
}

void CallManager::startCall()
{
    delete webrtc;
    createWebRTC("2", true);
    qDebug() << "Starting call with Caller ID:" << m_callerId;
}

void CallManager::endCall()
{
    qDebug() << "Ending call with Caller ID:" << m_callerId;
}

void CallManager::setUserName(QString userName)
{
    userInSignallingServer = userName;
}

QString CallManager::getCompletedJson(const QString& description, const QString type)
{
    QJsonDocument doc = QJsonDocument::fromJson(description.toUtf8());

    if (!doc.isObject())
        qWarning() << "Invalid JSON format";

    QJsonObject jsonObj = doc.object();
    jsonObj["reqType"] = "offer";
    jsonObj["user"] = userInSignallingServer;
    jsonObj["target"] = m_callerId;

    QJsonDocument updatedDoc(jsonObj);
    return updatedDoc.toJson(QJsonDocument::Compact);
}

void CallManager::createWebRTC(const QString &id, bool isOfferer)
{
    webrtc = new WebRTC();
    webrtc->init(id, isOfferer);
    webrtc->addPeer(id);

    connect(webrtc, &WebRTC::offerIsReady, [this](const QString &peerId, const QString& description) {
        QString updatedJsonString = getCompletedJson(description, "offer");
        socket->sendMessage(updatedJsonString);
    });

    connect(webrtc, &WebRTC::answerIsReady, [this](const QString &peerId, const QString& description) {
        QString updatedJsonString = getCompletedJson(description, "answer");
        socket->sendMessage(updatedJsonString);
    });

    connect(webrtc, &WebRTC::incommingPacket, [this](const QString &peerId, const QByteArray &data, qint64 len) {
        // Process the packat
    });
}
