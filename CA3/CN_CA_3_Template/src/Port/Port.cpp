#include "Port.h"

Port::Port(QObject *parent) :
    QObject(parent), m_number(0), m_numberOfPacketsSent(0)
{}

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

QString Port::routerIp()
{
    return m_routerIP;
}

uint8_t Port::number()
{
    return m_number;
}
