#include "PC.h"

PC::PC(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent) {
    m_port = QSharedPointer<Port>::create(0);
    connect(this, &PC::sendPacket, m_port.data(), &Port::sendPacket, Qt::AutoConnection);
    connect(m_port.data(), &Port::packetReceived, this, &PC::receivePacket, Qt::AutoConnection);
}

PC::~PC() {}

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

PortPtr_t PC::getIdlePort() {
    if (m_port->state() == UT::PortState::Idle) return m_port;
    else return nullptr;
}
