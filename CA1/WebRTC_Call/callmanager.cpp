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

    connect(webrtc, &WebRTC::offerIsReady, [this](const QString &peerId, const QString& description) {
        qDebug() << "OFFER::";
        QString updatedJsonString = getCompletedJson(description, "offer");
        socket->sendMessage(updatedJsonString);
    });

    connect(webrtc, &WebRTC::answerIsReady, [this](const QString &peerId, const QString& description) {
        QString updatedJsonString = getCompletedJson(description, "answer");
        socket->sendMessage(updatedJsonString);
    });

    connect(webrtc, &WebRTC::incommingPacket, [this](const QString &peerId, const QByteArray &data, qint64 len) {
        // Process the packat
        qDebug() << "fuckkkkkkkkkkkkk!!!!!!!!!!!!!!";
    });
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
    delete webrtc;
    createWebRTC("2", true);
    qDebug() << "Starting call with Caller ID:" << m_callerId;
}

void CallManager::endCall()
{
    qDebug() << "Ending call with Caller ID:" << m_callerId;
}

void CallManager::registerUser()
{
    QString jsonReq;
    jsonReq = createJsonRequest({"reqType", "user"}, {"register", m_userName});
    qDebug() << "!!!!!!!!!!!!!!! " << jsonReq << " !!!!!!!!!!!!!!! " << m_userName;
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

QString CallManager::getCompletedJson(const QString& description, const QString type)
{
    QJsonDocument doc = QJsonDocument::fromJson(description.toUtf8());

    if (!doc.isObject())
        qWarning() << "Invalid JSON format";

    QJsonObject jsonObj = doc.object();

    jsonObj["reqType"] = "offer";
    jsonObj["user"] = m_userName;
    jsonObj["target"] = m_callerId;

    QJsonDocument updatedDoc(jsonObj);
    return updatedDoc.toJson(QJsonDocument::Compact);
}

void CallManager::createWebRTC(const QString &id, bool isOfferer)
{
    webrtc = new WebRTC();
    webrtc->init(id, isOfferer);
    webrtc->addPeer(id);
}
