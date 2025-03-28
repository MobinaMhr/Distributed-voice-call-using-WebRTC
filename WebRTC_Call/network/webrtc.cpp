#include "webrtc.h"

static_assert(true);

#pragma pack(push, 1)
struct RtpHeader {
    uint8_t first;
    uint8_t marker:1;
    uint8_t payloadType:7;
    uint16_t sequenceNumber;
    uint32_t timestamp;
    uint32_t ssrc;
};
#pragma pack(pop)

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

WebRTC::~WebRTC()
{}


// ================= public methods =================== //

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
            qDebug() << "WebRTC: Peer connection state: New";
            break;
        case rtc::PeerConnection::State::Connecting:
            qDebug() << "WebRTC: Peer connection state: Connecting" ;
            break;
        case rtc::PeerConnection::State::Connected:
            Q_EMIT connectionStablished();
            qDebug() << "WebRTC: Peer connection state: Connected" ;
            break;
        case rtc::PeerConnection::State::Disconnected:
            qDebug() << "WebRTC: Peer connection state: Disconnected" ;
            break;
        case rtc::PeerConnection::State::Failed:
            qDebug() << "WebRTC: Peer connection state: Failed" ;
            break;
        case rtc::PeerConnection::State::Closed:
            Q_EMIT connectionClosed();
            qDebug() << "WebRTC: Peer connection state: Closed" ;
            break;
        }
    });

    pc->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::New)
            qDebug() << "WebRTC: state is New";
        if (state == rtc::PeerConnection::GatheringState::InProgress)
            qDebug() << "WebRTC: state is inProgress";
        if (state == rtc::PeerConnection::GatheringState::Complete){
            qDebug() << "WebRTC: state is complete";
            Q_EMIT gatheringComplited(peerId);
        }
    });

    addAudioTrack(peerId, QString::fromStdString("Hasti:("));
    pc->setLocalDescription();
}

void WebRTC::generateOfferSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
}

void WebRTC::generateAnswerSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
}

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
        qWarning() << "WebRTC: Error sending track: " << e.what();
    }
}


// ================= public slots ===================== //

void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    rtc::Description::Type type = (m_isOfferer) ? rtc::Description::Type::Answer
                                                : rtc::Description::Type::Offer;
    rtc::Description remoteDescription(sdp.toStdString(), type);

    m_peerConnections[peerID]->setRemoteDescription(remoteDescription);
}

void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    rtc::Candidate remoteCandidate(candidate.toStdString(), sdpMid.toStdString());
    m_peerConnections[peerID]->addRemoteCandidate(remoteCandidate);
}


// ================= private methods ================== //

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

int WebRTC::bitRate() const
{
    return m_bitRate;
}

void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate == newBitRate)
        return;

    m_bitRate = newBitRate;
    Q_EMIT bitRateChanged();
}

void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
}

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

        qDebug() << "WebRTC: Peer connection closed for peer:" << peerId;

    } else {
        qWarning() << "WebRTC: No peer connection found for peer:" << peerId;
    }
}

std::vector<rtc::Candidate> WebRTC::getCandidates(const QString &peerID)
{
    return m_peerConnections[peerID]->localDescription()->candidates();
}

QString WebRTC::getMid(const QString &peerID)
{
    return QString::fromStdString(m_peerConnections[peerID]->localDescription()->candidates()[0].mid());
}

void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType == newPayloadType)
        return;

    m_payloadType = newPayloadType;
    Q_EMIT payloadTypeChanged();
}

void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
}

rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc == newSsrc)
        return;

    m_ssrc = newSsrc;
    Q_EMIT ssrcChanged();
}

void WebRTC::resetSsrc()
{
    m_ssrc = 2;
}

int WebRTC::payloadType() const
{
    return m_payloadType;
}


// ================= getters setters ================== //

bool WebRTC::isOfferer() const
{
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer)
{
    if (m_isOfferer == newIsOfferer)
        return;

    m_isOfferer = newIsOfferer;
    Q_EMIT isOffererChanged();
}

void WebRTC::resetIsOfferer()
{
    m_isOfferer = false;
}
