#include "Packet.h"

using namespace UT;

Packet::Packet(UT::PacketType packetType, UT::PacketControlType controlType, quint32 seqNumber, quint32 waitCycles,
       quint32 totalCycles, const QString destIP, const QByteArray &payload, const DataLinkHeader &dataLinkHeader,
       const TCPHeader &tcpHeader, QObject *parent)
    : QObject(parent), m_packetType(packetType), m_controlType(controlType), m_sequenceNumber(seqNumber),
    m_waitingCycles(waitCycles), m_totalCycles(totalCycles), m_destinationIP(destIP), m_payload(payload),
    m_dataLinkHeader(dataLinkHeader), m_tcpHeader(tcpHeader) {}

void Packet::updateRoute(const QString& ipAddress) {
    m_route.append(ipAddress);
}
void Packet::increaseWaitingCyclesByOne() {
    m_waitingCycles = m_waitingCycles + 1;
}
void Packet::increaseTotalCyclesByOne() {
    m_totalCycles = m_totalCycles + 1;
}

// Getters
UT::PacketType Packet::packetType() const {
    return m_packetType;
}
UT::PacketControlType Packet::controlType() const {
    return m_controlType;
}
QStringList Packet::route() const {
    return m_route;
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
QByteArray Packet::payload() const {
    return m_payload;
}
const DataLinkHeader& Packet::dataLinkHeader() const {
    return m_dataLinkHeader;
}
const TCPHeader& Packet::tcpHeader() const {
    return m_tcpHeader;
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
