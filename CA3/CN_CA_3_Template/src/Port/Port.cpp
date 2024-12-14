#include "Port.h"

Port::Port(uint8_t number, QObject *parent) :
    QObject(parent),
    m_number(number),
    m_numberOfPacketsSent(0),
    m_state(UT::PortState::Idle)
{}


UT::IPVersion   m_ipVersion;
IPv4_t          m_ipv4Address;
IPv6_t          m_ipv6Address;

Port::Port(const Port &other)
    : QObject(other.parent()),
    m_number(other.m_number),
    m_numberOfPacketsSent(other.m_numberOfPacketsSent),
    m_ipVersion(other.m_ipVersion)
{}

// Port& Port::operator=(const Port &other)
// {
//     if (this != &other) {
//         m_number = other.m_number;
//         m_numberOfPacketsSent = other.m_numberOfPacketsSent;
//         m_ipVersion = other.m_ipVersion;
//         QObject::setParent(other.parent());
//     }
//     return *this;
// }

Port::~Port() {}

void Port::sendPacket(const PacketPtr_t &data, int portNumber)
{
    if (portNumber == m_number || portNumber == BROADCAST_ON_ALL_PORTS){
        ++m_numberOfPacketsSent;
        Q_EMIT packetSent(data);

    }
}

void Port::receivePacket(const PacketPtr_t &data)
{
    Q_EMIT packetReceived(data);
}

uint8_t Port::number()
{
    return m_number;
}

QString Port::ipAddress()
{
    return (m_ipVersion == UT::IPVersion::IPv4) ?
             ipv4Address() :
             ipv6Address();
}


QString Port::ipv6Address() const
{
    return m_ipv6Address.toString();
}

QString Port::ipv4Address() const {
    return m_ipv4Address.toString();
}

UT::PortState Port::state() {
    return m_state;
}

void Port::setState(UT::PortState state) {
    m_state = state;
}
