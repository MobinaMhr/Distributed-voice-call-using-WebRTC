#include "Port.h"

Port::Port(QObject *parent) : // uint8_t number,
    QObject(parent),
    m_number(0),  ///////////////////////// TODO
    m_numberOfPacketsSent(0),
    m_state(UT::PortState::Idle)
{}

Port::Port(const Port &other)
    : QObject(other.parent()),
    m_number(other.m_number),
    m_numberOfPacketsSent(other.m_numberOfPacketsSent),
    m_ipAddress(other.m_ipAddress)
{}

Port& Port::operator=(const Port &other)
{
    if (this != &other) {
        m_number = other.m_number;
        m_numberOfPacketsSent = other.m_numberOfPacketsSent;
        m_ipAddress = other.m_ipAddress;
        QObject::setParent(other.parent());
    }
    return *this;
}

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

QString Port::ipAddress()
{
    return m_ipAddress;
}

uint8_t Port::number()
{
    return m_number;
}

void Port::setIpAddress(QString ipAddress) {
    m_ipAddress = ipAddress;
}

UT::PortState Port::state() {
    return m_state;
}

void Port::setState(UT::PortState state) {
    m_state = state;
}
