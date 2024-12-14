#include "PC.h"

PC::PC(int id, const MacAddress &macAddress, QThread *parent)
    : Node(id, macAddress, parent) {
    m_port = QSharedPointer<Port>::create();
    connect(this, &PC::sendPacket, m_port.data(), &Port::sendPacket, Qt::AutoConnection);
    connect(m_port.data(), &Port::packetReceived, this, &PC::receivePacket, Qt::AutoConnection);
} //،TODO create pc port !!

PC::~PC() {}

QString PC::ipv6Address() const
{
    return m_ipv6Address.toString();
}

QString PC::ipv4Address() const {
    return m_ipv4Address.toString();
}

void PC::receivePacket(const PacketPtr_t &packet) {
    bool isMine = false;
    UT::IPVersion packetIpVersion = packet->ipVersion();
    if (packetIpVersion == UT::IPVersion::IPv4){
        if (m_ipv4Address.toString() == packet->ipv4Header().destIp())
            isMine = true;
    }
    else if(packetIpVersion == UT::IPVersion::IPv6){
        if (m_ipv6Address.toString() == packet->ipv6Header().destIp())
            isMine = true;
    }

    if (isMine){
        qDebug() << "PC" << name() << "recieved :";
        packet->print();
    }
    // Process the packet based on specific PC logic
}
