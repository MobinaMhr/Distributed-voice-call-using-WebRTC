#include "Packet.h"

using namespace UT;

Packet::Packet(UT::PacketType packetType, UT::PacketControlType controlType, quint32 seqNumber, quint32 waitCycles,
       quint32 totalCycles, const QString destIP, const QByteArray &payload, const DataLinkHeader &dataLinkHeader,
       const TCPHeader &tcpHeader, IPHv4_t ipv4Header, IPHv6_t ipv6Header, QObject *parent)
    : QObject(parent),           m_packetType(packetType),
    m_controlType(controlType),  m_destinationIP(destIP),
    m_ipv4Header(ipv4Header),    m_ipv6Header(ipv6Header),
    m_tcpHeader(tcpHeader),      m_dataLinkHeader(dataLinkHeader),
    m_payload(payload),          m_sequenceNumber(seqNumber),
    m_waitingCycles(waitCycles), m_totalCycles(totalCycles) {}

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
QString Packet::destinationIP() const {
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
