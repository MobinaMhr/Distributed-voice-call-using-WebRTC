#include "PC.h"

PC::PC(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent) {
    m_port = QSharedPointer<Port>::create(0);
    connect(this, &PC::sendPacket, m_port.data(), &Port::sendPacket, Qt::AutoConnection);
    connect(m_port.data(), &Port::packetReceived, this, &PC::receivePacket, Qt::AutoConnection);
}

PC::~PC() {}

void PC::receivePacket(const PacketPtr_t &packet, uint8_t portNumber) {
    if (!packet) {
        qDebug() << name() << ": Received a null packet.";
        return;
    }
    packet->decreasePacketTtl();
    if (m_hasIP){
        if (!isPacketMine(packet) && !packet->shouldDrop()) {
            return;
        }//TODO:should move somewhere else;
    }

    switch (packet->packetType()) {
        case UT::PacketType::Control:
            processControlPacket(packet, portNumber);
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

void PC::handleDhcpOffer(PacketPtr_t packet)
{
    if (packet->dataLinkHeader().source().toString() == m_macAddress.toString()){
        if (m_ipv4Address->toValue() == DEFAULT_IP_VALUE){
            QString sugestedIp = packet->readStringFromPayload();
            IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
            QByteArray payload ;
            DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                       packet->dataLinkHeader().destination());
            TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
            IPHv4_t *iphv4 = new IPHv4_t();
            IPHv6_t *iphv6 = new IPHv6_t();
            Packet *req = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPRequest, 1, 0, 0,
                                               fakeDest, payload, *dh, *th, *iphv4, *iphv6, DHCP_TTL);
            req->storeIntInPayload(m_id);
            PacketPtr_t reqPt = PacketPtr_t(req);
            Q_EMIT sendPacket(reqPt, BROADCAST_ON_ALL_PORTS);//send at clock tick
        }
    }
}

void PC::handleDhcpAck(PacketPtr_t packet)
{
    QString payload = packet->readStringFromPayload();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    QString ip = jsonDoc.object().value("ip").toString();
    QString mask = jsonDoc.object().value("mask").toString();
    setIP(ip, mask);
}

PortPtr_t PC::getIdlePort() {
    if (m_port->state() == UT::PortState::Idle) return m_port;
    else return nullptr;
}

void PC::processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) {
    if (m_ipv4Address->isInSubnet(packet->ipv4Header().sourceIp())){
        switch (packet->controlType()) {
            case UT::PacketControlType::DHCPOffer:
                handleDhcpOffer(packet);
                break;

            case UT::PacketControlType::DHCPAcknowledge:
                handleDhcpAck(packet);
                break;

            default:
                break;
        }
    }
}

void PC::processDataPacket(const PacketPtr_t &packet) {
    qDebug() << name() << ": Implement data packet handling logic.";
    // Add specific logic for Data Packet handling, such as forwarding or delivering.
}

void PC::route(UT::PacketControlType protocol)
{
    QJsonObject helloPacketPayload;
    helloPacketPayload["type"] = HELLO;
    IPHv4_t v4 = *(new IPHv4_t());
    v4.setSourceIp(m_ipv4Address);
    IPHv6_t v6 = *(new IPHv6_t());
    v6.setSourceIp(m_ipv4Address->toIPv6());
    IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
    QByteArray payload;
    DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, this->m_macAddress);
    TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
    if (protocol == UT::PacketControlType::OSPF){
        helloPacketPayload["router"] = (m_ipVersion == UT::IPVersion::IPv4) ? m_ipv4Address->toString() :
                                         m_ipv6Address->toString();
        QJsonDocument doc(helloPacketPayload);
        QString payloadStrng = QString(doc.toJson(QJsonDocument::Compact));
        Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::OSPF,
                                           1, 0, 0, fakeDest, payload, *dh, *th, v4, v6,
                                           OSPF_TTL);

        hello->storeStringInPayload(payloadStrng);
        // buffer it to send!!!
    }else {
        Packet *hello = new Packet(UT::PacketType::Control, UT::PacketControlType::RIP,
                                            1, 0, 0, fakeDest, payload, *dh, *th, v4, v6,
                                            RIP_TTL);
        QJsonArray nodesArray;
        QJsonArray costsArray;
        helloPacketPayload["nodes"] = nodesArray;
        helloPacketPayload["costs"] = costsArray;
        QJsonDocument doc(helloPacketPayload);
        QString payloadStrng = QString(doc.toJson(QJsonDocument::Compact)) ;
        hello->storeStringInPayload(payloadStrng);
        // buffer packet
    }
}
