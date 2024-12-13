#include "Port.h"

Port::Port(QObject *parent) : // uint8_t number,
    QObject(parent), m_number(0), m_numberOfPacketsSent(0)
{}

// Copy Constructor
Port::Port(const Port &other)
    : QObject(other.parent()), // Copy the parent QObject if necessary
    m_number(other.m_number),
    m_numberOfPacketsSent(other.m_numberOfPacketsSent),
    m_ipAddress(other.m_ipAddress)
{
    // If you have any other members that require deep copies, handle them here
}

// Assignment Operator
Port& Port::operator=(const Port &other)
{
    if (this != &other) {
        m_number = other.m_number;
        m_numberOfPacketsSent = other.m_numberOfPacketsSent;
        m_ipAddress = other.m_ipAddress;
        // Copy other members if necessary

        // Copy QObject parent if needed
        QObject::setParent(other.parent());
    }
    return *this;
}

Port::~Port() {}

void Port::sendPacket(const PacketPtr_t &data)
{
    ++m_numberOfPacketsSent;
    Q_EMIT packetSent(data);
}

void Port::receivePacket(const PacketPtr_t &data)
{
    // Process received packet (optional)
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
