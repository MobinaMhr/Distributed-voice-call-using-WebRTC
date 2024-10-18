#include "webrtc.h"
#include <QtEndian>
#include <QJsonDocument>

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
    : QObject{parent}, m_audio("Audio")
{
    connect(this, &WebRTC::gatheringComplited, [this](const QString &peerID) {
        m_localDescription = descriptionToJson(m_peerConnections[peerID]->localDescription().value());
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);

        if (m_isOfferer)
            Q_EMIT offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT answerIsReady(peerID, m_localDescription);
    });
}

WebRTC::~WebRTC() {}

void WebRTC::init(const QString &id, bool isOfferer)
{
    m_localId = id;
    m_isOfferer = isOfferer;

    rtc::Configuration config;
    config.iceServers.push_back({"stun:stun.l.google.com:19302"});
    m_config = config;

    qDebug() << "WebRTC initialized for ID:" << m_localId << " (isOfferer: " << m_isOfferer << ")";
}

void WebRTC::addPeer(const QString &peerId)
{
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections[peerId] = newPeer;

    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        QString sdp = QString::fromStdString(description.sdp());
        m_peerSdps[peerId] = description;
        if (description.typeString() == "offer") {
            Q_EMIT offerIsReady(peerId, sdp);
        } else {
            Q_EMIT answerIsReady(peerId, sdp);
        }
    });

    newPeer->onLocalCandidate([this, peerId](const rtc::Candidate &candidate) {
        QString candidateStr = QString::fromStdString(candidate.candidate());
        QString sdpMid = QString::fromStdString(candidate.mid());
        Q_EMIT localCandidateGenerated(peerId, candidateStr, sdpMid);
    });

    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        if (state == rtc::PeerConnection::State::Connected) {
            qDebug() << "Peer" << peerId << "connected.";
        } else if (state == rtc::PeerConnection::State::Disconnected) {
            qDebug() << "Peer" << peerId << "disconnected.";
        }
    });

    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            Q_EMIT gatheringComplited(peerId);
        }
    });

    newPeer->onTrack([this, peerId](std::shared_ptr<rtc::Track> track) {
        track->onMessage([this, peerId](rtc::message_variant data) {
            QByteArray packet = readVariant(data);
            Q_EMIT incommingPacket(peerId, packet, packet.size());
        });
    });

    qDebug() << "Peer" << peerId << "added.";
}

void WebRTC::generateOfferSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
        qDebug() << "Generated SDP offer for peer" << peerId;
    }
}

void WebRTC::generateAnswerSDP(const QString &peerId)
{
    if (m_peerConnections.contains(peerId)) {
        m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
        qDebug() << "Generated SDP answer for peer" << peerId;
    }
}

void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    if (m_peerConnections.contains(peerId)) {
        auto track = m_peerConnections[peerId]->addTrack(m_audio);
        m_peerTracks[peerId] = track;

        track->onMessage([this, peerId](rtc::message_variant data) {
            QByteArray packet = readVariant(data);
            Q_EMIT incommingPacket(peerId, packet, packet.size());
        });

        track->onFrame([this](rtc::binary frame, rtc::FrameInfo info) {
            // Handle received frame (optional for video or audio processing)
        });

        qDebug() << "Audio track" << trackName << "added to peer" << peerId;
    }
}



void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    if (m_peerTracks.contains(peerId)) {
        RtpHeader rtpHeader;
        rtpHeader.first = 0x80;
        rtpHeader.marker = 1;
        rtpHeader.payloadType = m_payloadType;
        rtpHeader.sequenceNumber = qToBigEndian(m_sequenceNumber++);
        rtpHeader.timestamp = qToBigEndian(getCurrentTimestamp());
        rtpHeader.ssrc = qToBigEndian(m_ssrc);

        QByteArray rtpPacket;
        rtpPacket.append(reinterpret_cast<const char*>(&rtpHeader), sizeof(RtpHeader));
        rtpPacket.append(buffer);

        m_peerTracks[peerId]->send(rtpPacket.toStdString());

        qDebug() << "Sent RTP packet to peer" << peerId;
    }
}

void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    if (m_peerConnections.contains(peerID)) {
        rtc::Description remoteDescription(sdp.toStdString(), rtc::Description::Type::Offer);
        m_peerConnections[peerID]->setRemoteDescription(remoteDescription);
        qDebug() << "Remote description set for peer" << peerID;
    }
}

void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    if (m_peerConnections.contains(peerID)) {
        rtc::Candidate remoteCandidate(candidate.toStdString(), sdpMid.toStdString(), 0);
        m_peerConnections[peerID]->addRemoteCandidate(remoteCandidate);
        qDebug() << "Remote candidate added for peer" << peerID;
    }
}

QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{
    if (auto binary = std::get_if<rtc::binary>(&data)) {
        return QByteArray(reinterpret_cast<const char*>(binary->data()), binary->size());
    }
    return QByteArray();
}

QString WebRTC::descriptionToJson(const rtc::Description &description)
{
    QJsonObject json;
    json["sdp"] = QString::fromStdString(description.sdp());
    json["type"] = QString::fromStdString(description.typeString());
    return QString(QJsonDocument(json).toJson(QJsonDocument::Compact));
}

int WebRTC::bitRate() const { return m_bitRate; }
void WebRTC::setBitRate(int newBitRate) { m_bitRate = newBitRate; emit bitRateChanged(); }
void WebRTC::resetBitRate() { m_bitRate = 48000; emit bitRateChanged(); }

int WebRTC::payloadType() const { return m_payloadType; }
void WebRTC::setPayloadType(int newPayloadType) { m_payloadType = newPayloadType; emit payloadTypeChanged(); }
void WebRTC::resetPayloadType() { m_payloadType = 111; emit payloadTypeChanged(); }

rtc::SSRC WebRTC::ssrc() const { return m_ssrc; }
void WebRTC::setSsrc(rtc::SSRC newSsrc) { m_ssrc = newSsrc; emit ssrcChanged(); }
void WebRTC::resetSsrc() { m_ssrc = 2; emit ssrcChanged(); }

bool WebRTC::isOfferer() const { return m_isOfferer; }
void WebRTC::setIsOfferer(bool newIsOfferer) { m_isOfferer = newIsOfferer; emit isOffererChanged(); }
void WebRTC::resetIsOfferer() { m_isOfferer = false; emit isOffererChanged(); }

inline uint32_t WebRTC::getCurrentTimestamp()
{
    using namespace std::chrono;
    auto now = steady_clock::now();
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count();
    return static_cast<uint32_t>(ms);
}
