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

        m_localDescription = descriptionToJson(m_peerConnections[peerID]->localDescription().value());
        Q_EMIT localDescriptionGenerated(peerID, m_localDescription);

        if (m_isOfferer)
            Q_EMIT this->offerIsReady(peerID, m_localDescription);
        else
            Q_EMIT this->answerIsReady(peerID, m_localDescription);
    });
}

WebRTC::~WebRTC()
{}


/**
 * ====================================================
 * ================= public methods ===================
 * ====================================================
 */

void WebRTC::init(const QString &id, bool isOfferer)
{
    // Initialize WebRTC using libdatachannel library
    m_isOfferer = isOfferer;
    m_localId = id;

    // Create an instance of rtc::Configuration to Set up ICE configuration
    rtc::Configuration config;

    // Add a STUN server to help peers find their public IP addresses
    config.iceServers.emplace_back("stun:stun.l.google.com:19302");

    // Add a TURN server for relaying media if a direct connection can't be established
    config.iceServers.emplace_back("turn:165.232.44.143", "myturn", "mewmew"); // TODO: can be turn:YOUR_TURN_SERVER_URL?transport=tcp

    m_config = config;

    // Set up the audio stream configuration
    // TODO

    // Create a peer connection
    auto peerConnection = std::make_shared<rtc::PeerConnection>(config);
    m_peerConnections[id] = peerConnection;

    std::shared_ptr<rtc::AudioSource> audioSource = std::make_shared<rtc::AudioSource>();

    rtc::AudioTrack::Config audioConfig;
    audioConfig.codec = rtc::AudioCodec::Opus;
    audioConfig.sampleRate = 48000;
    audioConfig.channels = 1;  // Mono audio

    // Set up callbacks for the peer connection (e.g., for ICE candidates and SDP)
    addPeer(id);

    // Set up the audio stream configuration
    addAudioTrack(id, "Audio");

    // Emit a signal to indicate initialization is complete
    Q_EMIT initializationCompleted(id);
}

void WebRTC::addPeer(const QString &peerId)
{
    // Create and add a new peer connection
    auto newPeer = m_peerConnections[peerId];


    // Set up a callback for when the local description is generated

    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        // The local description should be emitted using the appropriate signals based on the peer's role (offerer or answerer)
        Q_EMIT localDescriptionGenerated(peerId, descriptionToJson(description));
    });

    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        QString candidateString = QString::fromStdString(candidate.candidate());
        QString midString = QString::fromStdString(candidate.mid());
        Q_EMIT localCandidateGenerated(peerId, candidateString, midString);
    });

    // Set up a callback for when the state of the peer connection changes
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        // Handle different states like New, Connecting, Connected, Disconnected, etc.
        switch (state) {
        case rtc::PeerConnection::State::New:
            qDebug() << "Peer connection: New";
            break;
        case rtc::PeerConnection::State::Connecting:
            qDebug() << "Peer connection: Connecting";
            break;
        case rtc::PeerConnection::State::Connected:
            qDebug() << "Peer connection: Connected";
            // Q_EMIT connected(peerId);
            break;
        case rtc::PeerConnection::State::Disconnected:
            qDebug() << "Peer connection: Disconnected";
            // Q_EMIT disconnected(peerId);
            break;
        case rtc::PeerConnection::State::Failed:
            qDebug() << "Peer connection: Failed";
            // Q_EMIT connectionFailed(peerId);
            break;
        case rtc::PeerConnection::State::Closed:
            break;
        }
    });

    // Set up a callback for monitoring the gathering state
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        // When the gathering is complete, emit the gatheringComplited signal
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            Q_EMIT gatheringComplited(peerId);
        }
    });

    // Set up a callback for handling incoming tracks
    newPeer->onTrack([this, peerId] (std::shared_ptr<rtc::Track> track) {
        Q_EMIT incommingPacket(peerId, )
        Q_EMIT incomingPacket(peerId, track->id().c_str());
        // // handle the incoming media stream, emitting the incommingPacket signal if a stream is received
        // if (track->kind() == rtc::Track::Kind::Audio) {
        //     Q_EMIT incommingPacket(peerId, )
        //     Q_EMIT incomingPacket(peerId, track->id().c_str());
        // }
    });

    // Add an audio track to the peer connection
    addAudioTrack(peerId, "Audio");
}

// Set the local description for the peer's connection
void WebRTC::generateOfferSDP(const QString &peerId)
{
    auto peer = m_peerConnections[peerId];
    peer->setLocalDescription(rtc::Description::Type::Offer);
}

// Generate an answer SDP for the peer
void WebRTC::generateAnswerSDP(const QString &peerId)
{
    auto peer = m_peerConnections[peerId];
    peer->setLocalDescription(rtc::Description::Type::Answer);
}

// Add an audio track to the peer connection
void WebRTC::addAudioTrack(const QString &peerId, const QString &trackName)
{
    // Add an audio track to the peer connection

    // Handle track events

    track->onMessage([this, peerId] (rtc::message_variant data) {

    });

    track->onFrame([this] (rtc::binary frame, rtc::FrameInfo info) {

    });

}

void WebRTC::sendTrack(const QString &peerId, const QByteArray &buffer)
{
    // Create the RTP header and initialize an RtpHeader struct
    RtpHeader rtpHeader;
    rtpHeader.first = 0x80;
    rtpHeader.marker = 1;
    rtpHeader.payloadType = 96; // Dynamic payload type for audio
    rtpHeader.sequenceNumber = qToBigEndian(static_cast<uint16_t>(rand()));  // Random sequence number
    rtpHeader.timestamp = qToBigEndian(static_cast<uint32_t>(QDateTime::currentMSecsSinceEpoch()));
    rtpHeader.ssrc = qToBigEndian(static_cast<uint32_t>(rand())); // Random SSRC

    // Create the RTP packet by appending the RTP header and the payload buffer
    QByteArray rtpPacket(reinterpret_cast<const char*>(&rtpHeader), sizeof(RtpHeader));
    rtpPacket.append(buffer);

    // Send the packet, catch and handle any errors that occur during sending
    auto track = m_peerTracks.value(peerId);
    if (!track) {
        qWarning() << "Audio track not found for peer:" << peerId;
        return;
    }
    try {
        track->send(rtpPacket.constData());
    } catch (const std::exception &e) {
        qWarning() << "Error sending RTP packet:" << e.what();
    }
}


/**
 * ====================================================
 * ================= public slots =====================
 * ====================================================
 */

// Set the remote SDP description for the peer that contains metadata about the media being transmitted
void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp)
{

}

// Add remote ICE candidates to the peer connection
void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid)
{

}


/*
 * ====================================================
 * ================= private methods ==================
 * ====================================================
 */

// Utility function to read the rtc::message_variant into a QByteArray
QByteArray WebRTC::readVariant(const rtc::message_variant &data)
{

}

// Utility function to convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description)
{

}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{

}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{

}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{

}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{

}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{

}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{

}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{

}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{

}

// Retrieve the current payload type
int WebRTC::payloadType() const
{

}


/**
 * ====================================================
 * ================= getters setters ==================
 * ====================================================
 */

bool WebRTC::isOfferer() const
{

}

void WebRTC::setIsOfferer(bool newIsOfferer)
{

}

void WebRTC::resetIsOfferer()
{

}


