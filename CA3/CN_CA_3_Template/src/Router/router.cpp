#include "Router.h"
#include "qdebug.h"

const QString LOG_TITLE = "Router:: ";
const int MAX_BUFFER_SIZE = 20;

Router::Router(int id, const MacAddress &macAddress, int portCount, UT::IPVersion ipVersion, int bufferSize, QThread *parent)
    : Node(id, macAddress, portCount, ipVersion, parent),
    m_routing_table(new RoutingTable),
    m_ports(portCount),
    m_bufferSize(bufferSize),
    m_dhcp(nullptr),
    m_isRouting(false),
    m_rip((new RIP(IPv4Ptr_t::create((new IPv4_t("255.255.255.255", DEFAULT_MASK, this))),
                    MacAddress(DEFAULT_MAC)))),
    m_ospf(new OSPF(IPv4Ptr_t::create((new IPv4_t("255.255.255.255", DEFAULT_MASK, this))), m_macAddress, this)){
        for (int i = 0; i < portCount; ++i) {
        m_ports[i] = QSharedPointer<Port>::create(i);

        connect(this, &Router::sendPacket, m_ports[i].data(), &Port::sendPacket, Qt::AutoConnection);
        connect(m_ports[i].data(), &Port::packetReceived, this, &Router::receivePacket, Qt::AutoConnection);
    }
}

Router::~Router() {}

void Router::addRoutingEntry(IpPtr_t &destinationIp, IpPtr_t &nextHopIp, QSharedPointer<Port> &port, int metric) {
    m_routing_table->addRoute(destinationIp, nextHopIp, port, "TODO::", metric);
}

bool Router::isBufferAtCapacity() {
    return m_buffer.size() >= MAX_BUFFER_SIZE;
}

int Router::findBufferPositionForPacket(UT::PacketType packetType) {
    if (packetType == UT::PacketType::Data) {
        if (!isBufferAtCapacity()) {
            return static_cast<int>(m_buffer.size());
        }
    } else if (packetType == UT::PacketType::Control) {
        auto it = std::find_if(m_buffer.rbegin(), m_buffer.rend(), [](const PacketPtr_t &pkt) {
            return pkt->packetType() == UT::PacketType::Control;
        });

        /// Potential Bugs. TODO:: fix the findBuff for Control Packets.

        if (it != m_buffer.rend()) {
            if (isBufferAtCapacity()) m_buffer.pop_back();
            return static_cast<int>(it.base() - m_buffer.begin());
        } else if (!isBufferAtCapacity()) {
            return -1;
        }
    }

    return -1;
}

void Router::handleDhcpDiscovery(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        int id = packet->readIntFromPayload();
        QString sugestedIp = m_dhcp->assignIPToNode(id);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress, packet->dataLinkHeader().destination());

        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *offer = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPOffer,
                                           1, 0, 0, fakeDest, payload, *dh, *th, *iphv4, *iphv6,
                                           DHCP_TTL);
        offer->storeStringInPayload(sugestedIp);
        PacketPtr_t offerPt = PacketPtr_t(offer);
        /// BUFFER HERE instead of send

        // Q_EMIT sendPacket(offerPt, BROADCAST_ON_ALL_PORTS);
        // generate offer packet
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}

void Router::handleDhcpOffer(PacketPtr_t packet)
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
            Q_EMIT sendPacket(reqPt, BROADCAST_ON_ALL_PORTS);
        }
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}

void Router::handleDhcpReq(PacketPtr_t packet)
{
    if (m_dhcp != nullptr){
        QString response = createDhcpAckBody(packet);
        IpPtr_t fakeDest = IPv4_t::createIpPtr("255.255.255.255", "255.255.255.255");
        QByteArray payload ;
        DataLinkHeader *dh = new DataLinkHeader(this->m_macAddress,
                                                   packet->dataLinkHeader().destination());
        TCPHeader *th = new TCPHeader(BROADCAST_ON_ALL_PORTS, BROADCAST_ON_ALL_PORTS);
        IPHv4_t *iphv4 = new IPHv4_t();
        IPHv6_t *iphv6 = new IPHv6_t();
        Packet *ack = new Packet(UT::PacketType::Control, UT::PacketControlType::DHCPAcknowledge,
                                           1, 0, 0, fakeDest, payload, *dh, *th, *iphv4, *iphv6,
                                           DHCP_TTL);
        ack->storeStringInPayload(response);
        PacketPtr_t ackPt = PacketPtr_t(ack);
        Q_EMIT sendPacket(ackPt, BROADCAST_ON_ALL_PORTS);
        // generate offer packet
    }
    else
        Q_EMIT sendPacket(packet, BROADCAST_ON_ALL_PORTS);
}

void Router::handleDhcpAck(PacketPtr_t packet)
{
    QString payload = packet->readStringFromPayload();
    QJsonDocument jsonDoc = QJsonDocument::fromJson(payload.toUtf8());
    QString ip = jsonDoc.object().value("ip").toString();
    QString mask = jsonDoc.object().value("mask").toString();
    setIP(ip, mask);
}

void Router::receivePacket(const PacketPtr_t &packet, uint8_t portNumber) {
    if (!packet) {
        qDebug() << name() << ": Received a null packet.";
        return;
    }
    packet->decreasePacketTtl();

    if (!isPacketMine(packet) && !packet->shouldDrop()) {
        bufferPacket(packet);
        return;
    }//TODO:should move somewhere else;

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

void Router::finishRouting(RoutingTable routingTable)
{
    m_routing_table = new RoutingTable(routingTable);
    m_isRouting = false;
    Q_EMIT routingFinished(m_id);
}

void Router::getIP()
{
    if (m_dhcp != nullptr){
        QString sugestedIp = m_dhcp->assignIPToNode(m_id);
        setIP(sugestedIp, DEFAULT_MASK);
    }else
        sendDiscovery();
}

void Router::handleTick(int cycleNumebt)
{
    if (m_isRouting){
        handleRoutingPacket();
    }
}

QString Router::createDhcpAckBody(PacketPtr_t packet)
{
    int id = packet->readIntFromPayload();
    QString sugestedIp = m_dhcp->assignIPToNode(id);
    QJsonObject jsonObject;
    jsonObject["ip"] = sugestedIp;
    jsonObject["mask"] = DEFAULT_MASK;

    // Convert the JSON object to a QJsonDocument
    QJsonDocument jsonDoc(jsonObject);

    // Convert the JSON document to a QString
    QString jsonString = jsonDoc.toJson(QJsonDocument::Compact);

    return jsonString;
}

void Router::handleRoutingPacket()
{
    if (m_protocol == UT::PacketControlType::RIP){
        if (m_rip->isUpdateReady()){
            Packet *update = new Packet(m_rip->getUpdatePacket());
            Q_EMIT sendPacket(PacketPtr_t(update), BROADCAST_ON_ALL_PORTS);
        }
    }
    else if(m_protocol == UT::PacketControlType::OSPF){
        for (int i = 0 ; i < m_buffer.size(); i++){
            Q_EMIT sendPacket(m_buffer[i], BROADCAST_ON_ALL_PORTS);
        }
        m_buffer.clear();
        if (m_ospf->isLSAReady()){
            Packet *lsa = new Packet(m_ospf->getlsaPacket());
            Q_EMIT sendPacket(PacketPtr_t(lsa), BROADCAST_ON_ALL_PORTS);
        }
    }
}

void Router::handleOspfPacket(PacketPtr_t packet, int portNumber)
{
    QString payload = packet->readStringFromPayload();
    QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
    QString type = doc.object()["type"].toString();
    if (type != HELLO)
        bufferPacket(packet);
    m_ospf->handleOSPFPacket(packet, m_ports[portNumber]);
}

void Router::bufferPacket(const PacketPtr_t &packet) {
    int position = findBufferPositionForPacket(packet->packetType());

    if (position == -1) {
        qDebug() << LOG_TITLE << "Packet Dropped: Buffer Full. Sequence:" << packet->sequenceNumber();
        return;
    }

    if (position == static_cast<int>(m_buffer.size())) {
        m_buffer.push_back(std::move(packet));
    } else {
        m_buffer.insert(m_buffer.begin() + position, std::move(packet));
    }

    qDebug() << LOG_TITLE << (packet->packetType() == UT::PacketType::Data ?
                                "Data Packet Added." : "Control Packet Added.")
             << "Buffer Size:" << m_buffer.size();
}

void Router::routePackets() {
    for (auto it = m_buffer.begin(); it != m_buffer.end(); /* no increment here */) {
        const auto& packetPtr = *it;

        if (!packetPtr) {
            qDebug() << "Encountered a null packet pointer.";
            it = m_buffer.erase(it);
            continue;
        }

        qDebug() << "Processing packet with sequence number:" << packetPtr->sequenceNumber();

        if (!m_routing_table->routeExists(packetPtr->destinationIP())) {
            qDebug() << "No Route Exists for the packet";
            continue;
        }

        QSharedPointer<Port> hostPort = m_routing_table->getPort(packetPtr->destinationIP());
        int portIndex = hostPort->number();

        if (m_ports[portIndex]->state() == UT::PortState::Reserved) {
            m_ports[portIndex]->setState(UT::PortState::Sending);
            qDebug() << "Routing packet to port" << portIndex;

            Q_EMIT sendPacket(packetPtr, hostPort->number());
            it = m_buffer.erase(it);
        } else if (m_ports[portIndex]->state() == UT::PortState::Sending) {
            qDebug() << "Port" << portIndex << "is already sending. Keeping packet in buffer.";
            ++it;
        } else {
            qDebug() << "Port" << portIndex << "is not bined and is idle.";
            ++it;
        }

        bool allPortsBusy = std::all_of(
          m_ports.begin(),
          m_ports.end(),
          [](PortPtr_t port) { return (port->state() == UT::PortState::Sending) ||
                                      (port->state() == UT::PortState::Idle); });

        if (allPortsBusy) {
            qDebug() << "All ports are Busy. Stopping packet routing.";
            break;
        }
    }
}

PortPtr_t Router::getIdlePort() {
    for (auto port : m_ports) {
        if (port->state() == UT::PortState::Idle)
            return port;
    }
    return nullptr;
}

std::vector<PortPtr_t> Router::getPorts() {
    return m_ports;
}

void Router::setDhcp(int asNumber)
{
    this->m_dhcp = new DHCP(asNumber);
}

void Router::route(UT::PacketControlType protocol)
{
    m_isRouting = true;
    m_protocol = protocol;
    if (m_protocol == UT::PacketControlType::RIP){
        m_rip = new RIP(m_ipv4Address, m_macAddress, this);
        connect(m_rip, &RIP::routingFinished, this, &Router::finishRouting, Qt::AutoConnection);
        m_rip->run();
    }else if(protocol == UT::PacketControlType::OSPF){
        m_ospf = new OSPF(m_ipv4Address, m_macAddress, this);
        connect(m_ospf, &OSPF::routingFinished, this, &Router::finishRouting, Qt::AutoConnection);
    }
}

std::vector<QSharedPointer<Node>> Router::neighbors() {
    return m_neighbors;
}

void Router::processControlPacket(const PacketPtr_t &packet, uint8_t portNumber) {
    qDebug() << name() << ": Implement control packet handling logic.";
    if (m_ipv4Address->isInSubnet(packet->ipv4Header().sourceIp())){
        switch (packet->controlType()) {
            case UT::PacketControlType::DHCPDiscovery:
                handleDhcpDiscovery(packet);
                break;

            case UT::PacketControlType::DHCPOffer:
                handleDhcpOffer(packet);
                break;

            case UT::PacketControlType::DHCPRequest:
                handleDhcpReq(packet);
                break;

            case UT::PacketControlType::DHCPAcknowledge:
                handleDhcpAck(packet);
                break;
            case UT::PacketControlType::RIP:
                m_rip->handleRIPPacket(packet, m_ports[portNumber]);
                break;
            case UT::PacketControlType::OSPF:
                handleOspfPacket(packet, portNumber);
                break;

            default:
                break;
        }
    }
    // Add specific logic for Control Packet processing (e.g., routing updates, acknowledgments).
}

void Router::processDataPacket(const PacketPtr_t &packet) {
    qDebug() << name() << ": Implement data packet handling logic.";
    // Add specific logic for Data Packet handling, such as forwarding or delivering.
}
