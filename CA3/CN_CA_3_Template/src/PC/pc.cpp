#include "PC.h"

PC::PC(int id, const MacAddress &macAddress, const IPv4_t &ipAddress, QThread *parent)
    : Node(id, macAddress, parent), m_ipAddress(ipAddress.toValue()) {}

PC::~PC() {}

QString PC::ipAddress() const {
    return m_ipAddress.toString();
}

void PC::sendPacket(const Packet &packet) {
    qDebug() << "PC" << name();
    // Logic to send a packet using IP and MAC headers
}

void PC::receivePacket(const Packet &packet) {
    qDebug() << "PC" << name();

    // Process the packet based on specific PC logic
}
