#include "PC.h"

PC::PC(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent) {
    m_port = QSharedPointer<Port>::create(0);
    connect(this, &PC::sendPacket, m_port.data(), &Port::sendPacket, Qt::AutoConnection);
    connect(m_port.data(), &Port::packetReceived, this, &PC::receivePacket, Qt::AutoConnection);
}

PC::~PC() {}

void PC::receivePacket(const PacketPtr_t &packet) {
    if (!packet) {
        qDebug() << name() << ": Received a null packet.";
        return;
    }
    if (!isPacketMine(packet)) {
        return;
    }

    switch (packet->packetType()) {
        case UT::PacketType::Control:
            processControlPacket(packet);
            break;

        case UT::PacketType::Data:
            processDataPacket(packet);
            break;

        default:
            break;
    }
}

void PC::getIP()
{
    sendDiscovery();
}

PortPtr_t PC::getIdlePort() {
    if (m_port->state() == UT::PortState::Idle) return m_port;
    else return nullptr;
}

void PC::processControlPacket(const PacketPtr_t &packet) {
    switch (packet->controlType()) {
        case UT::PacketControlType::Request:

            break;
        case UT::PacketControlType::Response:

            break;
        case UT::PacketControlType::Acknowledge:

            break;
        case UT::PacketControlType::Error:

            break;
        case UT::PacketControlType::DHCPDiscovery:

            break;
        case UT::PacketControlType::DHCPOffer:

            break;
        case UT::PacketControlType::DHCPRequest:

            break;
        case UT::PacketControlType::DHCPAcknowledge:

            break;
        case UT::PacketControlType::DHCPNak:

            break;
        case UT::PacketControlType::RIP:
        case UT::PacketControlType::OSPF:

            break;
        default:
            break;
    }
}

void PC::processDataPacket(const PacketPtr_t &packet) {
    qDebug() << name() << ": Implement data packet handling logic.";
    // Add specific logic for Data Packet handling, such as forwarding or delivering.
}
