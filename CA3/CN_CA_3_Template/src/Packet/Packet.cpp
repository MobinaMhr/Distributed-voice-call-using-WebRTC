#include "Packet.h"

using namespace UT;

const std::map<PacketControlType, QString> packetControlTypeStrings = {
 {PacketControlType::Request, "Request"},
 {PacketControlType::Response, "Response"},
 {PacketControlType::Acknowledge, "Acknowledge"},
 {PacketControlType::Error, "Error"},
 {PacketControlType::DHCPDiscovery, "DHCPDiscovery"},
 {PacketControlType::DHCPOffer, "DHCPOffer"},
 {PacketControlType::DHCPRequest, "DHCPRequest"},
 {PacketControlType::DHCPAcknowledge, "DHCPAcknowledge"},
 {PacketControlType::DHCPNak, "DHCPNak"},
 {PacketControlType::RIP, "RIP"},
 {PacketControlType::OSPF, "OSPF"},
 };

QString toString(PacketControlType type) {
    auto it = packetControlTypeStrings.find(type);
    if (it != packetControlTypeStrings.end()) {
        return it->second;
    }
    return "Unknown";
}


Packet::Packet(UT::PacketType packetType, UT::PacketControlType controlType, quint32 seqNumber, quint32 waitCycles,
               quint32 totalCycles, IpPtr_t destIP, const QByteArray &payload,
               const DataLinkHeader &dataLinkHeader, const TCPHeader &tcpHeader,
               IPHv4_t ipv4Header, IPHv6_t ipv6Header, QObject *parent)
    : QObject(parent), m_packetType(packetType), m_controlType(controlType),
    m_destinationIP(destIP), m_ipv4Header(ipv4Header), m_ipv6Header(ipv6Header),
    m_tcpHeader(tcpHeader), m_dataLinkHeader(dataLinkHeader), m_payload(payload),
    m_sequenceNumber(seqNumber), m_waitingCycles(waitCycles), m_totalCycles(totalCycles) {}

void Packet::updatePath(const QString& ipAddress) {
    m_path.append(ipAddress);
}

void Packet::increaseWaitingCyclesBy(int additionalCycles) {
    m_waitingCycles += additionalCycles;
}
void Packet::increaseTotalCyclesBy(int additionalCycles) {
    m_totalCycles = m_totalCycles + additionalCycles;
}

void Packet::increaseWaitingCycles() {
    m_waitingCycles += 1;
}
void Packet::increaseTotalCycles() {
    m_totalCycles = m_totalCycles + 1;
}

// Getters
UT::PacketType Packet::packetType() const {
    return m_packetType;
}
UT::PacketControlType Packet::controlType() const {
    return m_controlType;
}
QStringList Packet::path() const {
    return m_path;
}
quint32 Packet::sequenceNumber() const {
    return m_sequenceNumber;
}
quint32 Packet::waitingCycles() const {
    return m_waitingCycles;
}
quint32 Packet::totalCycles() const {
    return m_totalCycles;
}
IpPtr_t Packet::destinationIP() const {
    return m_destinationIP;
}
QByteArray Packet::payload() const {
    return m_payload;
}
const DataLinkHeader& Packet::dataLinkHeader() const {
    return m_dataLinkHeader;
}
const TCPHeader& Packet::tcpHeader() const {
    return m_tcpHeader;
}
UT::IPVersion Packet::ipVersion() const {
    return m_ipVersion;
}
const IPHv4_t Packet::ipv4Header() const {
    return m_ipv4Header;
}
const IPHv6_t Packet::ipv6Header() const {
    return m_ipv6Header;
}

// Setters
void Packet::setPacketType(UT::PacketType packetType) {
    m_packetType = packetType;
}
void Packet::setControlType(UT::PacketControlType controlType) {
    m_controlType = controlType;
}
void Packet::setSequenceNumber(quint32 seqNumber) {
    m_sequenceNumber = seqNumber;
}
void Packet::setPayload(const QByteArray& payload) {
    m_payload = payload;
}
void Packet::setDataLinkHeader(const DataLinkHeader& dataLinkHeader) {
    m_dataLinkHeader = dataLinkHeader;
}
void Packet::setTcpHeader(const TCPHeader& tcpHeader) {
    m_tcpHeader = tcpHeader;
}

void Packet::print()
{
    QString packetType = (m_packetType == UT::PacketType::Data) ? " Data" : " Control";
    qDebug() << "Packet Details:";
    qDebug() << "Packet Type:" << packetType;
    qDebug() << "Control Type:" << toString(m_controlType);
    qDebug() << "Sequence Number:" << m_sequenceNumber;
    qDebug() << "Waiting Cycles:" << m_waitingCycles;
    qDebug() << "Total Cycles:" << m_totalCycles;
    qDebug() << "Payload Size:" << m_payload.size() << "bytes";
    qDebug() << "Path:" << m_path.join(" -> ");
    qDebug() << "IP Version:" << (m_ipVersion == UT::IPVersion::IPv4 ? "IPv4" : "IPv6");
    if (m_ipVersion == UT::IPVersion::IPv4){
        qDebug() << "IPv4 Header:";
        m_ipv4Header.print();
    }
    else{
        qDebug() << "IPv6 Header:";
        m_ipv6Header.print();
    }
    m_tcpHeader.print();
}
