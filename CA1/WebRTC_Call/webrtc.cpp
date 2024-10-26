#include "webrtc.h"

// TODO::Connect signals and methods
// TODO::we are not sure about the usage of trackName in addAudioTrack method

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

// incommingPacket bere vase audioInput output
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

    connect(this, &WebRTC::localDescriptionGenerated, [this] (const QString &peerID){ // , const QString &m_localDescription
        if (m_isOfferer)
            generateOfferSDP(peerID);
        else
            generateAnswerSDP(peerID);
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
    // Initialize WebRTC using libdatachannel library
    rtc::InitLogger(rtc::LogLevel::Debug);

    m_localId = id;
    m_isOfferer = isOfferer;

    rtc::Configuration config;

    m_config = config;
    // Create an instance of rtc::Configuration to Set up ICE configuration
    // Add a STUN server to help peers find their public IP addresses
    m_config.iceServers.emplace_back("stun:165.232.44.143:3478");

    // Add a TURN server for relaying media if a direct connection can't be established
    m_config.iceServers.emplace_back("turn:165.232.44.143:3478", "myturn", "mewmew");

    // Set up the audio stream configuration
    m_audio.addOpusCodec(m_payloadType);
    m_audio.setBitrate(m_bitRate);
    m_audio.addSSRC(m_ssrc, "audio-send");
    m_audio.setDirection(rtc::Description::Direction::SendRecv);
}

void WebRTC::addPeer(const QString &peerId)
{
    // Create and add a new peer connection
    auto pc = std::make_shared<rtc::PeerConnection>();
    m_peerConnections[peerId] = pc;

    // Set up a callback for when the local description is generated
    pc->onLocalDescription([this, peerId](const rtc::Description &description) {
        // The local description should be emitted using the appropriate signals based on the peer's role (offerer or answerer)
        Q_EMIT localDescriptionGenerated(peerId, QString::fromStdString(description)); // description?? or description.value??
    });

    // Set up a callback for handling local ICE candidates
    pc->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        // Emit the local candidates using the localCandidateGenerated signal
        Q_EMIT localCandidateGenerated(peerId, QString::fromStdString(candidate.candidate()),
                                       QString::fromStdString(candidate.mid())); // potential bugssss!!!!!!!!!!!
    });

    // Set up a callback for when the state of the peer connection changes
    pc->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        // Handle different states like New, Connecting, Connected, Disconnected, etc.
        switch(state) {
            case rtc::PeerConnection::State::New:
                qDebug() << "Peer connection state: New";
                break;
            case rtc::PeerConnection::State::Connecting:
                qDebug() << "Peer connection state: Connecting" ;
                break;
            case rtc::PeerConnection::State::Connected:
                qDebug() << "Peer connection state: Connected" ;
                break;
            case rtc::PeerConnection::State::Disconnected:
                qDebug() << "Peer connection state: Disconnected" ;
                break;
            case rtc::PeerConnection::State::Failed:
                qDebug() << "Peer connection state: Failed" ;
                break;
            case rtc::PeerConnection::State::Closed:
                qDebug() << "Peer connection state: Closed" ;
                break;
        }

    });

    // Set up a callback for monitoring the gathering state
    pc->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        // When the gathering is complete, emit the gatheringComplited signal
        if (state == rtc::PeerConnection::GatheringState::Complete)
            Q_EMIT gatheringComplited(peerId);
    });

    // Set up a callback for handling incoming tracks
    pc->onTrack([this, peerId] (std::shared_ptr<rtc::Track> track) {
        qDebug() << "Hastiiiiiiiiiiiiiiiiiiiiiiiiiiiiii!" ;
    });

    addAudioTrack(peerId, QString::fromStdString("Hasti:("));
}

// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Offer);
}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{
    m_peerConnections[peerId]->setLocalDescription(rtc::Description::Type::Answer);
}

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    // Add an audio track to the peer connection

    // Handle track events

    auto pc = m_peerConnections[peerId];

    // Create a track for the peer
    auto track = pc->addTrack(m_audio);
    m_peerTracks[peerId] = track;

    // track->onMessage([this, peerId] (rtc::message_variant data) {
    //     QByteArray receivedData = readVariant(data);
    //     // Process received RTP data, e.g., extract header and handle audio payload.
    // });

    // Listen for incoming frames
    track->onMessage([this, peerId](rtc::binary message){
        const QByteArray data = QByteArray(reinterpret_cast<const char*>(message.data()), message.size());
        Q_EMIT incommingPacket(peerId, data, data.size());
    }, nullptr);
}

// Sends audio track data to the peer
void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    // Create the RTP header and initialize an RtpHeader struct
    RtpHeader header;
    header.first = 0x80;
    header.marker = 0;
    header.payloadType = m_payloadType;
    header.sequenceNumber = qToBigEndian(m_sequenceNumber++);
    header.timestamp = qToBigEndian(getCurrentTimestamp());
    header.ssrc = qToBigEndian(m_ssrc);

    // Create the RTP packet by appending the RTP header and the payload buffer
    QByteArray rtpPacket;
    rtpPacket.append(reinterpret_cast<char*>(&header), sizeof(RtpHeader));
    rtpPacket.append(buffer);

    // Send the packet, catch and handle any errors that occur during sending

    const rtc::byte *data = reinterpret_cast<const rtc::byte *>(rtpPacket.constData());
    try {
        m_peerTracks[peerId]->send(data, rtpPacket.size());
    } catch (const std::exception &e) {
        // std::err << "Error sending track: " << e.what() << std::endl;
    }
}


// ================= public slots ===================== //

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{
    rtc::Description::Type type = (m_isOfferer) ? rtc::Description::Type::Answer : rtc::Description::Type::Offer;
    rtc::Description remoteDescription(sdp.toStdString(), type);

    m_peerConnections[peerID]->setRemoteDescription(remoteDescription);
}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{
    rtc::Candidate remoteCandidate(candidate.toStdString(), sdpMid.toStdString());
    m_peerConnections[peerID]->addRemoteCandidate(remoteCandidate);
}


// ================= private methods ================== //

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const QString &peerID)
{
    auto description = m_peerConnections[peerID]->localDescription();

    QJsonObject jsonMessage;

    jsonMessage["type"] = QString::fromStdString(description->typeString());

    jsonMessage["sdp"] = QString::fromStdString(description.value());

    QJsonDocument doc(jsonMessage);
    QString jsonString = doc.toJson(QJsonDocument::Compact);

    qDebug() << jsonString;

    return jsonString;
}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{
    return m_bitRate;
}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{
    m_bitRate = newBitRate;
}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{
    m_bitRate = 48000;
}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{
    m_payloadType = newPayloadType;
}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{
    m_payloadType = 111;
}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    m_ssrc = newSsrc;
}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{
    m_ssrc = 2;
}

// Retrieve the current payload type
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
    m_isOfferer = newIsOfferer;
}

void WebRTC::resetIsOfferer()
{
    m_isOfferer = false;
}

